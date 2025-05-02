#include "ConventionEngine/Kernel/CoreModule.h"
#include <mutex>
#include <thread>
#include <unordered_map>
#include <shared_mutex>

using namespace ConventionEngine;

/*
                               +---------------------+
                               | ConventionEngine    |
                               | Kernel              |
                               +----------+----------+
                                          |
                                          v
+------------------------+    +-----------------------+    +------------------------+
| CEObject               |    | RawT 内存池           |    | 线程本地缓存           |
+------------------------+    +-----------------------+    +------------------------+
| - name_max_length      |    | - RawMemoryBuffer     |    | - local_free_blocks    |
| - m_instanceID         |    | - RawMemoryBufferSize |    | - local_alloc_blocks   |
| - m_name               |    | - FreeBlockContainer  |    | - local_mutex          |
+------------------------+    | - AllocBlockContainer |    +------------------------+
| + operator new         |<-->| - RawObjectHandles    |<-->| 每个线程一个独立缓存   |
| + operator delete      |    +-----------------------+    +------------------------+
| + virtual ~CEObject()  |              |
+------------------------+              |
                                        v
                          +---------------------------+
                          |       锁机制              |
                          +---------------------------+
                          | - global_mutex (共享锁)   |
                          | - allocation_mutex        |
                          | - cache_mutex             |
                          +---------------------------+

+---------------------------------------------------------------------+
|                        内存分配流程                                 |
+---------------------------------------------------------------------+
                         |
                         v
              +------------------------+
              | MemoryAlloc(size)      |
              +------------------------+
                         |
                         v
         +----------------------------------+
         | 尝试从线程本地缓存分配           |     线程隔离，降低锁争用
         +----------------------------------+
                |                    |
      找到合适块 |                    | 没找到
                v                    v
    +-----------------------+    +-----------------------------+
    | 从本地缓存分配并返回  |    | 从全局内存池分配            |
    +-----------------------+    +-----------------------------+
                                              |
                                              v
                                  +------------------------+
                                  | 内存池是否有足够的块?  |
                                  +------------------------+
                                      |            |
                                  否  |            | 是
                                      v            v
                      +--------------------+  +----------------------+
                      | 执行GC整理内存碎片 |  | 分配内存并更新映射表 |
                      +--------------------+  +----------------------+
                                |
                                v
                      +----------------------+
                      | 内存池扩容           |
                      | (依据__RawT_Memory_K)|
                      +----------------------+

+---------------------------------------------------------------------+
|                        内存释放流程                                 |
+---------------------------------------------------------------------+
                         |
                         v
              +------------------------+
              | FreeMemory(ptr)       |
              +------------------------+
                         |
                         v
         +----------------------------------+
         | 检查是否在线程本地缓存中         |
         +----------------------------------+
                |                    |
            是  |                    | 否
                v                    v
    +-----------------------+    +-----------------------------+
    | 返回到本地空闲块列表  |    | 检查全局已分配块容器        |
    +-----------------------+    +-----------------------------+
                |                           |
                v                           v
    +-----------------------+    +-----------------------------+
    | 本地缓存是否已满?     |    | 尝试合并相邻空闲块          |
    +-----------------------+    | (减少内存碎片)              |
        |           |            +-----------------------------+
      是 |           | 否                    |
        v           v                        v
+----------------+ +---------------+  +-----------------------------+
| 返回部分到全局 | | 保留在本地    |  | 添加到全局空闲块容器        |
+----------------+ +---------------+  +-----------------------------+

+---------------------------------------------------------------------+
|                          GC流程                                     |
+---------------------------------------------------------------------+
                              |
                              v
                   +----------------------+
                   | GC_ConventionEngine()|
                   +----------------------+
                              |
                              v
            +----------------------------------+
            | 收集所有已分配的内存块           |
            | (全局+所有线程本地缓存)          |
            +----------------------------------+
                              |
                              v
                   +----------------------+
                   | 按地址排序内存块     |
                   +----------------------+
                              |
                              v
                   +----------------------+
                   | 创建新临时缓冲区     |
                   +----------------------+
                              |
                              v
            +----------------------------------+
            | 依次复制所有块到新缓冲区         |
            | (按排序顺序紧密排列)             |
            +----------------------------------+
                              |
                              v
            +----------------------------------+
            | 更新对象引用和内存块映射         |
            +----------------------------------+
                              |
                              v
                   +----------------------+
                   | 释放旧缓冲区         |
                   +----------------------+
*/


CEObject::~CEObject()
{
	// 虚析构函数实现为空是正常的
}

_Notnull_ constexpr const char* CEObject::GetName() const
{
	return this->m_name;
}

void CEObject::SetName(_In_ const char* name)
{
	string_indicator::c_strcpy_s(this->m_name, name, name_max_length);
}

#pragma region Memory

namespace RawT_Tools
{
	// 线程安全相关
	std::shared_mutex global_mutex; // 读写锁，用于保护全局内存池
	std::mutex allocation_mutex; // 用于保护分配操作
}

/*
class RawT :public any_class
{
private:
	CEHandle handle_tail = 0;

	std::map<CEHandle, CEObject*> RawObjectHandles = {};
	std::map<CEObject*, CEHandle> RawRObjectHandles = {};

	void* RawMemoryBuffer = nullptr;
	size_t RawMemoryBufferSize = 0;

	std::multimap<size_t, void*> FreeBlockContainer = {};
	std::map<void*, size_t> FreeRBlockContainer = {};
	std::map<void*, size_t> AllocBlockContainer = {};


	// 线程本地缓存 - 每个线程都有自己的内存池
	struct ThreadCache {
		std::multimap<size_t, void*> local_free_blocks; // 本地空闲块
		std::map<void*, size_t> local_alloc_blocks;    // 本地已分配块
		std::mutex local_mutex;                        // 本地锁
	};

	// 线程本地缓存映射表
	std::unordered_map<std::thread::id, ThreadCache> thread_caches;
	std::mutex cache_mutex; // 保护thread_caches映射表

	// 获取当前线程的缓存
	ThreadCache& GetThreadCache() {
		std::thread::id this_id = std::this_thread::get_id();
		std::lock_guard<std::mutex> lock(cache_mutex);
		return thread_caches[this_id];
	}

public:

	void Insert(_In_ CEObject* ptr)
	{
		std::lock_guard<std::shared_mutex> lock(RawT_Tools::global_mutex);
		RawObjectHandles[handle_tail] = ptr;
		RawRObjectHandles[ptr] = handle_tail;
		++handle_tail;
	}

	_Ret_maybenull_ CEObject* At(CEHandle handle)
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		if (handle < 0 && handle >= handle_tail)
			return nullptr;
		return RawObjectHandles[handle];
	}

	CEHandle At(_In_opt_ CEObject* ptr)
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		if (ptr == nullptr)
			return -1;
		return RawRObjectHandles[ptr];
	}

	bool Contains(CEHandle handle)
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		return RawObjectHandles.count(handle);
	}

	bool Contains(_In_opt_ CEObject* ptr)
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		if (ptr == nullptr)
			return false;
		return RawRObjectHandles.count(ptr);
	}

	friend void InitConventionEngine(size_t memory_size);
	friend void ClearConventionEngine();
	friend void QuitConventionEngine();
	friend void GC_ConventionEngine();

	_Ret_notnull_ void* MemoryAlloc(size_t size)
	{
		// 确保请求的内存大小不为0
		if (size == 0) size = sizeof(char);

		// 尝试从线程本地缓存分配
		#ifndef __RawT_Disable_Thread_Cache
		auto& thread_cache = GetThreadCache();
		std::lock_guard<std::mutex> local_lock(thread_cache.local_mutex);

		// 检查线程本地缓存是否有足够大的块
		auto local_it = thread_cache.local_free_blocks.lower_bound(size);
		if (local_it != thread_cache.local_free_blocks.end()) {
			auto&& [blocksize, ptr] = *local_it;

			// 从线程本地缓存中移除此块
			thread_cache.local_free_blocks.erase(local_it);

			// 如果块比需要的大，将剩余部分添加回本地缓存
			auto right_block_size = blocksize - size;
			if (right_block_size > sizeof(void*)) {
				void* new_free_ptr = static_cast<void*>((char*)ptr + size);
				thread_cache.local_free_blocks.insert({ right_block_size, new_free_ptr });
			}

			// 添加到本地已分配块
			thread_cache.local_alloc_blocks[ptr] = size;
			return ptr;
		}
		#endif

		// 线程本地缓存中没有合适的块，需要从全局池分配
		std::lock_guard<std::mutex> alloc_lock(RawT_Tools::allocation_mutex);

		// 如果没有足够的内存块，尝试GC
		if (FreeBlockContainer.empty() || FreeBlockContainer.rbegin()->first < size)
		{
			GC_ConventionEngine();

			// 再次检查是否有足够大的内存块
			if (FreeBlockContainer.empty() || FreeBlockContainer.rbegin()->first < size)
			{
				// 在GC后仍然没有足够的内存后重新分配
                size_t new_size = RawMemoryBufferSize * __RawT_Memory_K;
                // 确保新的大小足够容纳请求的内存块
                if (new_size - GetAllocatedMemory() < size)
                {
                    new_size = RawMemoryBufferSize + size * __RawT_Memory_K;
                }

                // 分配新的内存缓冲区
                void* new_buffer = ::malloc(new_size);
                if (new_buffer == nullptr)
                {
                    throw std::bad_alloc(); // 无法分配新内存，抛出异常
                }

                // 保存旧缓冲区信息
                void* old_buffer = RawMemoryBuffer;
                size_t old_size = RawMemoryBufferSize;

                // 收集所有已分配的内存块
                struct MemoryBlock {
                    void* ptr;
                    size_t size;
                    CEObject* object; // 如果是CEObject，保存指针，否则为nullptr
                };

                std::vector<MemoryBlock> allocated_blocks;

                // 收集全局已分配的内存块
                for (const auto& [ptr, size] : AllocBlockContainer)
                {
                    CEObject* obj = nullptr;
                    // 检查这个内存块是否是一个CEObject
                    for (const auto& [obj_ptr, _] : RawRObjectHandles)
                    {
                        if (obj_ptr == ptr)
                        {
                            obj = static_cast<CEObject*>(obj_ptr);
                            break;
                        }
                    }
                    allocated_blocks.push_back({ ptr, size, obj });
                }

                // 收集所有线程本地缓存中已分配的块
                {
                    std::lock_guard<std::mutex> cache_lock(cache_mutex);
                    for (auto& [thread_id, cache] : thread_caches) {
                        std::lock_guard<std::mutex> thread_lock(cache.local_mutex);
                        for (const auto& [ptr, size] : cache.local_alloc_blocks) {
                            CEObject* obj = nullptr;
                            // 检查这个内存块是否是一个CEObject
                            for (const auto& [obj_ptr, _] : RawRObjectHandles) {
                                if (obj_ptr == ptr) {
                                    obj = static_cast<CEObject*>(obj_ptr);
                                    break;
                                }
                            }
                            allocated_blocks.push_back({ ptr, size, obj });
                        }
                    }
                }

                // 重置内存管理容器
                AllocBlockContainer.clear();
                FreeBlockContainer.clear();
                FreeRBlockContainer.clear();

                // 清空所有线程本地缓存
                {
                    std::lock_guard<std::mutex> cache_lock(cache_mutex);
                    for (auto& [thread_id, cache] : thread_caches) {
                        std::lock_guard<std::mutex> thread_lock(cache.local_mutex);
                        cache.local_free_blocks.clear();
                        cache.local_alloc_blocks.clear();
                    }
                }

                // 更新内存缓冲区信息
                RawMemoryBuffer = new_buffer;
                RawMemoryBufferSize = new_size;

                // 当前写入位置
                size_t current_pos = 0;

                // 复制所有分配的块到新缓冲区
                for (const auto& block : allocated_blocks)
                {
                    // 复制内存
                    void* new_ptr = static_cast<void*>((char*)new_buffer + current_pos);
                    memcpy(new_ptr, block.ptr, block.size);

                    // 更新对象引用
                    if (block.object != nullptr)
                    {
                        // 更新映射
                        CEHandle handle = RawRObjectHandles[block.object];
                        RawObjectHandles[handle] = static_cast<CEObject*>(new_ptr);
                        RawRObjectHandles.erase(block.object);
                        RawRObjectHandles[static_cast<CEObject*>(new_ptr)] = handle;
                    }

                    // 更新AllocBlockContainer
                    AllocBlockContainer[new_ptr] = block.size;

                    // 更新位置
                    current_pos += block.size;
                }

                // 添加剩余内存作为单个空闲块
                void* free_ptr = static_cast<void*>((char*)new_buffer + current_pos);
                size_t free_size = new_size - current_pos;
                FreeBlockContainer.insert({ free_size, free_ptr });
                FreeRBlockContainer[free_ptr] = free_size;

                // 释放旧缓冲区
                ::free(old_buffer);
			}
		}

		void* result = nullptr;

		// 查找大小足够的最小块
		auto it = FreeBlockContainer.lower_bound(size);
		if (it != FreeBlockContainer.end())
		{
			auto&& [blocksize, ptr] = *it;
			result = ptr;

			// 从可用块容器中移除此块
			FreeBlockContainer.erase(it);
			FreeRBlockContainer.erase(ptr);

			// 如果块比需要的大，将剩余部分添加回可用块
			auto right_block_size = blocksize - size;
			if (right_block_size > sizeof(void*)) // 确保剩余块足够大
			{
				void* new_free_ptr = static_cast<void*>((char*)result + size);
				FreeBlockContainer.insert({ right_block_size, new_free_ptr });
				FreeRBlockContainer.insert({ new_free_ptr, right_block_size });
			}

			// 将分配的块添加到线程本地已分配块容器
			thread_cache.local_alloc_blocks[result] = size;
		}
		else
		{
			// 如果执行到这里，表示出现了意外情况
			throw std::bad_alloc();
		}

		assert(result != nullptr);
		return result;
	}

	void FreeMemory(_In_ void* ptr)
	{
		if (ptr == nullptr)
			return;

		// 首先检查是否在线程本地缓存中
		auto& thread_cache = GetThreadCache();
		{
			std::lock_guard<std::mutex> local_lock(thread_cache.local_mutex);
			auto local_it = thread_cache.local_alloc_blocks.find(ptr);
			if (local_it != thread_cache.local_alloc_blocks.end()) {
				// 在本地缓存中找到了
				size_t blocksize = local_it->second;
				thread_cache.local_alloc_blocks.erase(local_it);

				// 添加到本地空闲块
				thread_cache.local_free_blocks.insert({ blocksize, ptr });

				// 如果本地空闲块太多，可以考虑返回一部分给全局池
				const size_t MAX_LOCAL_FREE_BLOCKS = __RawT_Max_Thread_Cache_Free_Blocks; // 可调整的阈值
				if (thread_cache.local_free_blocks.size() > MAX_LOCAL_FREE_BLOCKS) {
					// 将最大的块返回给全局池
					std::lock_guard<std::mutex> alloc_lock(RawT_Tools::allocation_mutex);
					auto largest_it = thread_cache.local_free_blocks.rbegin();
					FreeBlockContainer.insert({ largest_it->first, largest_it->second });
					FreeRBlockContainer[largest_it->second] = largest_it->first;
					thread_cache.local_free_blocks.erase(--largest_it.base());
				}

				return;
			}
		}

		// 如果不在线程本地缓存中，则检查全局池
		std::lock_guard<std::mutex> alloc_lock(RawT_Tools::allocation_mutex);
		if (AllocBlockContainer.find(ptr) == AllocBlockContainer.end())
			return;

		// 获取块大小
		size_t blocksize = AllocBlockContainer[ptr];

		// 从已分配块容器中移除
		AllocBlockContainer.erase(ptr);

		// 尝试合并相邻的空闲块
		void* left_ptr = nullptr;
		void* right_ptr = static_cast<void*>((char*)ptr + blocksize);
		size_t merged_size = blocksize;

		// 检查右侧是否有空闲块
		if (FreeRBlockContainer.find(right_ptr) != FreeRBlockContainer.end())
		{
			size_t right_size = FreeRBlockContainer[right_ptr];
			// 从空闲块容器中移除右侧块
			auto range = FreeBlockContainer.equal_range(right_size);
			for (auto it = range.first; it != range.second; ++it)
			{
				if (it->second == right_ptr)
				{
					FreeBlockContainer.erase(it);
					break;
				}
			}
			FreeRBlockContainer.erase(right_ptr);
			merged_size += right_size;
		}

		// 检查左侧是否有空闲块
		// 这需要遍历所有空闲块，找到结束地址正好是ptr的块
		for (auto it = FreeRBlockContainer.begin(); it != FreeRBlockContainer.end(); ++it)
		{
			void* block_start = it->first;
			size_t block_size = it->second;
			void* block_end = static_cast<void*>((char*)block_start + block_size);

			if (block_end == ptr)
			{
				left_ptr = block_start;
				// 从空闲块容器中移除左侧块
				auto range = FreeBlockContainer.equal_range(block_size);
				for (auto bit = range.first; bit != range.second; ++bit)
				{
					if (bit->second == block_start)
					{
						FreeBlockContainer.erase(bit);
						break;
					}
				}
				FreeRBlockContainer.erase(block_start);
				merged_size += block_size;
				ptr = block_start; // 调整起始指针为左侧块的起始位置
				break;
			}
		}

		// 添加合并后的块到空闲块容器
		FreeBlockContainer.insert({ merged_size, ptr });
		FreeRBlockContainer[ptr] = merged_size;
	}

	// 获取当前内存使用情况的方法
	size_t GetTotalFreeMemory() const
	{
		std::shared_lock<decltype(RawT_Tools::global_mutex)> lock(RawT_Tools::global_mutex);
		size_t total = 0;

		// 统计全局空闲内存
		for (const auto& [size, _] : FreeBlockContainer)
		{
			total += size;
		}

		// 统计所有线程本地缓存的空闲内存
		std::lock_guard<std::mutex> cache_lock(const_cast<std::mutex&>(cache_mutex));
		for (const auto& [thread_id, cache] : thread_caches) {
			std::lock_guard<std::mutex> thread_lock(const_cast<std::mutex&>(cache.local_mutex));
			for (const auto& [size, _] : cache.local_free_blocks) {
				total += size;
			}
		}

		return total;
	}

	size_t GetLargestFreeBlock() const
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		size_t largest = 0;

		// 查找全局最大空闲块
		if (!FreeBlockContainer.empty()) {
			largest = FreeBlockContainer.rbegin()->first;
		}

		// 查找所有线程本地缓存中的最大空闲块
		std::lock_guard<std::mutex> cache_lock(const_cast<std::mutex&>(cache_mutex));
		for (const auto& [thread_id, cache] : thread_caches) {
			std::lock_guard<std::mutex> thread_lock(const_cast<std::mutex&>(cache.local_mutex));
			if (!cache.local_free_blocks.empty()) {
				largest = std::max(largest, cache.local_free_blocks.rbegin()->first);
			}
		}

		return largest;
	}

	size_t GetAllocatedMemory() const
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		size_t total = 0;

		// 统计全局已分配内存
		for (const auto& [_, size] : AllocBlockContainer)
		{
			total += size;
		}

		// 统计所有线程本地缓存的已分配内存
		std::lock_guard<std::mutex> cache_lock(const_cast<std::mutex&>(cache_mutex));
		for (const auto& [thread_id, cache] : thread_caches) {
			std::lock_guard<std::mutex> thread_lock(const_cast<std::mutex&>(cache.local_mutex));
			for (const auto& [_, size] : cache.local_alloc_blocks) {
				total += size;
			}
		}

		return total;
	}
};
*/

class RawT :public any_class
{
private:
	CEHandle handle_tail = 0;

	std::map<CEHandle, void*> RawObjectHandles = {};
	std::map<void*, CEHandle> RawRObjectHandles = {};

	void* RawMemoryBuffer = nullptr;
	size_t RawMemoryBufferSize = 0;

	std::multimap<size_t, void*> FreeBlockContainer = {};
	std::map<void*, size_t> FreeRBlockContainer = {};
	std::map<void*, size_t> AllocBlockContainer = {};


	// 线程本地缓存 - 每个线程都有自己的内存池
	struct ThreadCache {
		std::multimap<size_t, void*> local_free_blocks; // 本地空闲块
		std::map<void*, size_t> local_alloc_blocks;    // 本地已分配块
		std::mutex local_mutex;                        // 本地锁
	};

	// 线程本地缓存映射表
	std::unordered_map<std::thread::id, ThreadCache> thread_caches;
	std::mutex cache_mutex; // 保护thread_caches映射表

	// 获取当前线程的缓存
	ThreadCache& GetThreadCache() {
		std::thread::id this_id = std::this_thread::get_id();
		std::lock_guard<std::mutex> lock(cache_mutex);
		return thread_caches[this_id];
	}

public:

	void Insert(_In_ void* ptr)
	{
		std::lock_guard<std::shared_mutex> lock(RawT_Tools::global_mutex);
        if(ptr<RawMemoryBuffer || ptr>=static_cast<void*>((char*)RawMemoryBuffer+RawMemoryBufferSize))
            throw std::invalid_argument("ptr is out of range");
		RawObjectHandles[handle_tail] = ptr;
		RawRObjectHandles[ptr] = handle_tail;
		++handle_tail;
	}

	_Ret_maybenull_ void* At(CEHandle handle)
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		if (handle < 0 && handle >= handle_tail)
			return nullptr;
		return RawObjectHandles[handle];
	}

	CEHandle At(_In_opt_ void* ptr)
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		if (ptr == nullptr)
			return -1;
		return RawRObjectHandles[ptr];
	}

	bool Contains(CEHandle handle)
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		return RawObjectHandles.count(handle);
	}

	bool Contains(_In_opt_ void* ptr)
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		if (ptr == nullptr)
			return false;
		return RawRObjectHandles.count(ptr);
	}

	friend void InitConventionEngine(size_t memory_size);
	friend void ClearConventionEngine();
	friend void QuitConventionEngine();
	friend void GC_ConventionEngine();

	_Ret_notnull_ void* MemoryAlloc(size_t size)
	{
		// 确保请求的内存大小不为0
		if (size == 0) size = sizeof(char);

		// 尝试从线程本地缓存分配
#ifndef __RawT_Disable_Thread_Cache
		auto& thread_cache = GetThreadCache();
		std::lock_guard<std::mutex> local_lock(thread_cache.local_mutex);

		// 检查线程本地缓存是否有足够大的块
		auto local_it = thread_cache.local_free_blocks.lower_bound(size);
		if (local_it != thread_cache.local_free_blocks.end()) {
			auto&& [blocksize, ptr] = *local_it;

			// 从线程本地缓存中移除此块
			thread_cache.local_free_blocks.erase(local_it);

			// 如果块比需要的大，将剩余部分添加回本地缓存
			auto right_block_size = blocksize - size;
			if (right_block_size > sizeof(void*)) {
				void* new_free_ptr = static_cast<void*>((char*)ptr + size);
				thread_cache.local_free_blocks.insert({ right_block_size, new_free_ptr });
			}

			// 添加到本地已分配块
			thread_cache.local_alloc_blocks[ptr] = size;
			return ptr;
		}
#endif

		// 线程本地缓存中没有合适的块，需要从全局池分配
		std::lock_guard<std::mutex> alloc_lock(RawT_Tools::allocation_mutex);

		// 如果没有足够的内存块，尝试GC
		if (FreeBlockContainer.empty() || FreeBlockContainer.rbegin()->first < size)
		{
			GC_ConventionEngine();

			// 再次检查是否有足够大的内存块
			if (FreeBlockContainer.empty() || FreeBlockContainer.rbegin()->first < size)
			{
				// 在GC后仍然没有足够的内存后重新分配
				size_t new_size = RawMemoryBufferSize * __RawT_Memory_K;
				// 确保新的大小足够容纳请求的内存块
				if (new_size - GetAllocatedMemory() < size)
				{
					new_size = RawMemoryBufferSize + size * __RawT_Memory_K;
				}

				// 分配新的内存缓冲区
				void* new_buffer = ::malloc(new_size);
				if (new_buffer == nullptr)
				{
					throw std::bad_alloc(); // 无法分配新内存，抛出异常
				}

				// 保存旧缓冲区信息
				void* old_buffer = RawMemoryBuffer;
				size_t old_size = RawMemoryBufferSize;

				// 收集所有已分配的内存块
				struct MemoryBlock {
					void* ptr;
					size_t size;
					CEObject* object; // 如果是CEObject，保存指针，否则为nullptr
				};

				std::vector<MemoryBlock> allocated_blocks;

				// 收集全局已分配的内存块
				for (const auto& [ptr, size] : AllocBlockContainer)
				{
					CEObject* obj = nullptr;
					// 检查这个内存块是否是一个CEObject
					for (const auto& [obj_ptr, _] : RawRObjectHandles)
					{
						if (obj_ptr == ptr)
						{
							obj = static_cast<CEObject*>(obj_ptr);
							break;
						}
					}
					allocated_blocks.push_back({ ptr, size, obj });
				}

				// 收集所有线程本地缓存中已分配的块
				{
					std::lock_guard<std::mutex> cache_lock(cache_mutex);
					for (auto& [thread_id, cache] : thread_caches) {
						std::lock_guard<std::mutex> thread_lock(cache.local_mutex);
						for (const auto& [ptr, size] : cache.local_alloc_blocks) {
							CEObject* obj = nullptr;
							// 检查这个内存块是否是一个CEObject
							for (const auto& [obj_ptr, _] : RawRObjectHandles) {
								if (obj_ptr == ptr) {
									obj = static_cast<CEObject*>(obj_ptr);
									break;
								}
							}
							allocated_blocks.push_back({ ptr, size, obj });
						}
					}
				}

				// 重置内存管理容器
				AllocBlockContainer.clear();
				FreeBlockContainer.clear();
				FreeRBlockContainer.clear();

				// 清空所有线程本地缓存
				{
					std::lock_guard<std::mutex> cache_lock(cache_mutex);
					for (auto& [thread_id, cache] : thread_caches) {
						std::lock_guard<std::mutex> thread_lock(cache.local_mutex);
						cache.local_free_blocks.clear();
						cache.local_alloc_blocks.clear();
					}
				}

				// 更新内存缓冲区信息
				RawMemoryBuffer = new_buffer;
				RawMemoryBufferSize = new_size;

				// 当前写入位置
				size_t current_pos = 0;

				// 复制所有分配的块到新缓冲区
				for (const auto& block : allocated_blocks)
				{
					// 复制内存
					void* new_ptr = static_cast<void*>((char*)new_buffer + current_pos);
					memcpy(new_ptr, block.ptr, block.size);

					// 更新对象引用
					if (block.object != nullptr)
					{
						// 更新映射
						CEHandle handle = RawRObjectHandles[block.object];
						RawObjectHandles[handle] = static_cast<CEObject*>(new_ptr);
						RawRObjectHandles.erase(block.object);
						RawRObjectHandles[static_cast<CEObject*>(new_ptr)] = handle;
					}

					// 更新AllocBlockContainer
					AllocBlockContainer[new_ptr] = block.size;

					// 更新位置
					current_pos += block.size;
				}

				// 添加剩余内存作为单个空闲块
				void* free_ptr = static_cast<void*>((char*)new_buffer + current_pos);
				size_t free_size = new_size - current_pos;
				FreeBlockContainer.insert({ free_size, free_ptr });
				FreeRBlockContainer[free_ptr] = free_size;

				// 释放旧缓冲区
				::free(old_buffer);
			}
		}

		void* result = nullptr;

		// 查找大小足够的最小块
		auto it = FreeBlockContainer.lower_bound(size);
		if (it != FreeBlockContainer.end())
		{
			auto&& [blocksize, ptr] = *it;
			result = ptr;

			// 从可用块容器中移除此块
			FreeBlockContainer.erase(it);
			FreeRBlockContainer.erase(ptr);

			// 如果块比需要的大，将剩余部分添加回可用块
			auto right_block_size = blocksize - size;
			if (right_block_size > sizeof(void*)) // 确保剩余块足够大
			{
				void* new_free_ptr = static_cast<void*>((char*)result + size);
				FreeBlockContainer.insert({ right_block_size, new_free_ptr });
				FreeRBlockContainer.insert({ new_free_ptr, right_block_size });
			}

			// 将分配的块添加到线程本地已分配块容器
			thread_cache.local_alloc_blocks[result] = size;
		}
		else
		{
			// 如果执行到这里，表示出现了意外情况
			throw std::bad_alloc();
		}

		assert(result != nullptr);
		return result;
	}

	void FreeMemory(_In_ void* ptr)
	{
		if (ptr == nullptr)
			return;

		// 首先检查是否在线程本地缓存中
		auto& thread_cache = GetThreadCache();
		{
			std::lock_guard<std::mutex> local_lock(thread_cache.local_mutex);
			auto local_it = thread_cache.local_alloc_blocks.find(ptr);
			if (local_it != thread_cache.local_alloc_blocks.end()) {
				// 在本地缓存中找到了
				size_t blocksize = local_it->second;
				thread_cache.local_alloc_blocks.erase(local_it);

				// 添加到本地空闲块
				thread_cache.local_free_blocks.insert({ blocksize, ptr });

				// 如果本地空闲块太多，可以考虑返回一部分给全局池
				const size_t MAX_LOCAL_FREE_BLOCKS = __RawT_Max_Thread_Cache_Free_Blocks; // 可调整的阈值
				if (thread_cache.local_free_blocks.size() > MAX_LOCAL_FREE_BLOCKS) {
					// 将最大的块返回给全局池
					std::lock_guard<std::mutex> alloc_lock(RawT_Tools::allocation_mutex);
					auto largest_it = thread_cache.local_free_blocks.rbegin();
					FreeBlockContainer.insert({ largest_it->first, largest_it->second });
					FreeRBlockContainer[largest_it->second] = largest_it->first;
					thread_cache.local_free_blocks.erase(--largest_it.base());
				}

				return;
			}
		}

		// 如果不在线程本地缓存中，则检查全局池
		std::lock_guard<std::mutex> alloc_lock(RawT_Tools::allocation_mutex);
		if (AllocBlockContainer.find(ptr) == AllocBlockContainer.end())
			return;

		// 获取块大小
		size_t blocksize = AllocBlockContainer[ptr];

		// 从已分配块容器中移除
		AllocBlockContainer.erase(ptr);

		// 尝试合并相邻的空闲块
		void* left_ptr = nullptr;
		void* right_ptr = static_cast<void*>((char*)ptr + blocksize);
		size_t merged_size = blocksize;

		// 检查右侧是否有空闲块
		if (FreeRBlockContainer.find(right_ptr) != FreeRBlockContainer.end())
		{
			size_t right_size = FreeRBlockContainer[right_ptr];
			// 从空闲块容器中移除右侧块
			auto range = FreeBlockContainer.equal_range(right_size);
			for (auto it = range.first; it != range.second; ++it)
			{
				if (it->second == right_ptr)
				{
					FreeBlockContainer.erase(it);
					break;
				}
			}
			FreeRBlockContainer.erase(right_ptr);
			merged_size += right_size;
		}

		// 检查左侧是否有空闲块
		// 这需要遍历所有空闲块，找到结束地址正好是ptr的块
		for (auto it = FreeRBlockContainer.begin(); it != FreeRBlockContainer.end(); ++it)
		{
			void* block_start = it->first;
			size_t block_size = it->second;
			void* block_end = static_cast<void*>((char*)block_start + block_size);

			if (block_end == ptr)
			{
				left_ptr = block_start;
				// 从空闲块容器中移除左侧块
				auto range = FreeBlockContainer.equal_range(block_size);
				for (auto bit = range.first; bit != range.second; ++bit)
				{
					if (bit->second == block_start)
					{
						FreeBlockContainer.erase(bit);
						break;
					}
				}
				FreeRBlockContainer.erase(block_start);
				merged_size += block_size;
				ptr = block_start; // 调整起始指针为左侧块的起始位置
				break;
			}
		}

		// 添加合并后的块到空闲块容器
		FreeBlockContainer.insert({ merged_size, ptr });
		FreeRBlockContainer[ptr] = merged_size;
	}

	// 获取当前内存使用情况的方法
	size_t GetTotalFreeMemory() const
	{
		std::shared_lock<decltype(RawT_Tools::global_mutex)> lock(RawT_Tools::global_mutex);
		size_t total = 0;

		// 统计全局空闲内存
		for (const auto& [size, _] : FreeBlockContainer)
		{
			total += size;
		}

		// 统计所有线程本地缓存的空闲内存
		std::lock_guard<std::mutex> cache_lock(const_cast<std::mutex&>(cache_mutex));
		for (const auto& [thread_id, cache] : thread_caches) {
			std::lock_guard<std::mutex> thread_lock(const_cast<std::mutex&>(cache.local_mutex));
			for (const auto& [size, _] : cache.local_free_blocks) {
				total += size;
			}
		}

		return total;
	}

	size_t GetLargestFreeBlock() const
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		size_t largest = 0;

		// 查找全局最大空闲块
		if (!FreeBlockContainer.empty()) {
			largest = FreeBlockContainer.rbegin()->first;
		}

		// 查找所有线程本地缓存中的最大空闲块
		std::lock_guard<std::mutex> cache_lock(const_cast<std::mutex&>(cache_mutex));
		for (const auto& [thread_id, cache] : thread_caches) {
			std::lock_guard<std::mutex> thread_lock(const_cast<std::mutex&>(cache.local_mutex));
			if (!cache.local_free_blocks.empty()) {
				largest = std::max(largest, cache.local_free_blocks.rbegin()->first);
			}
		}

		return largest;
	}

	size_t GetAllocatedMemory() const
	{
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		size_t total = 0;

		// 统计全局已分配内存
		for (const auto& [_, size] : AllocBlockContainer)
		{
			total += size;
		}

		// 统计所有线程本地缓存的已分配内存
		std::lock_guard<std::mutex> cache_lock(const_cast<std::mutex&>(cache_mutex));
		for (const auto& [thread_id, cache] : thread_caches) {
			std::lock_guard<std::mutex> thread_lock(const_cast<std::mutex&>(cache.local_mutex));
			for (const auto& [_, size] : cache.local_alloc_blocks) {
				total += size;
			}
		}

		return total;
	}
};

RawT Raw;

_Ret_notnull_ void* ConventionEngineMemoryAlloc(size_t size)
{
	return Raw.MemoryAlloc(size);
}

void ConventionEngineFreeMemory(_In_ void* ptr)
{
	Raw.FreeMemory(ptr);
}

void InitConventionEngine(size_t memory_size)
{
	std::lock_guard<std::shared_mutex> lock(RawT_Tools::global_mutex);

	Raw.RawMemoryBufferSize = memory_size;
	Raw.RawMemoryBuffer = ::malloc(sizeof(char) * Raw.RawMemoryBufferSize);
	if (Raw.RawMemoryBuffer == nullptr)
	{
		// 处理内存分配失败
		Raw.RawMemoryBufferSize = 0;
		return;
	}

	// 初始化时整个内存块都是空闲的
	Raw.FreeBlockContainer.insert({ Raw.RawMemoryBufferSize, Raw.RawMemoryBuffer });
	Raw.FreeRBlockContainer[Raw.RawMemoryBuffer] = Raw.RawMemoryBufferSize;

	// 清空其他容器
	Raw.AllocBlockContainer.clear();
	Raw.RawObjectHandles.clear();
	Raw.RawRObjectHandles.clear();

	// 清空线程缓存
	std::lock_guard<std::mutex> cache_lock(Raw.cache_mutex);
	Raw.thread_caches.clear();
}

void ClearConventionEngine()
{
	std::lock_guard<std::shared_mutex> lock(RawT_Tools::global_mutex);

	// 删除所有对象
	for (auto&& [_, ptr] : Raw.RawObjectHandles)
	{
		delete ptr;
	}

	// 清空所有容器
	Raw.RawObjectHandles.clear();
	Raw.RawRObjectHandles.clear();
	Raw.AllocBlockContainer.clear();
	Raw.FreeBlockContainer.clear();
	Raw.FreeRBlockContainer.clear();

	// 清空线程缓存
	std::lock_guard<std::mutex> cache_lock(Raw.cache_mutex);
	Raw.thread_caches.clear();

	// 重置内存池状态
	memset(Raw.RawMemoryBuffer, 0, Raw.RawMemoryBufferSize);
	Raw.FreeBlockContainer.insert({ Raw.RawMemoryBufferSize, Raw.RawMemoryBuffer });
	Raw.FreeRBlockContainer[Raw.RawMemoryBuffer] = Raw.RawMemoryBufferSize;
}

void QuitConventionEngine()
{
	std::lock_guard<std::shared_mutex> lock(RawT_Tools::global_mutex);

	// 确保所有对象都被删除
	for (auto&& [_, ptr] : Raw.RawObjectHandles)
	{
		delete ptr;
	}

	// 清空所有容器
	Raw.RawObjectHandles.clear();
	Raw.RawRObjectHandles.clear();
	Raw.AllocBlockContainer.clear();
	Raw.FreeBlockContainer.clear();
	Raw.FreeRBlockContainer.clear();

	// 清空线程缓存
	std::lock_guard<std::mutex> cache_lock(Raw.cache_mutex);
	Raw.thread_caches.clear();

	// 释放内存池
	::free(Raw.RawMemoryBuffer);
	Raw.RawMemoryBuffer = nullptr;
	Raw.RawMemoryBufferSize = 0;
}

void GC_ConventionEngine()
{
	std::lock_guard<std::shared_mutex> lock(RawT_Tools::global_mutex);

	// 这个函数不会自动回收CEObject，而是通过整理内存来减少碎片

	// 1. 收集所有已分配的内存块
	struct MemoryBlock {
		void* ptr;
		size_t size;
		CEObject* object; // 如果是CEObject，保存指针，否则为nullptr
	};

	std::vector<MemoryBlock> allocated_blocks;

	// 收集全局已分配的内存块
	for (const auto& [ptr, size] : Raw.AllocBlockContainer)
	{
		CEObject* obj = nullptr;
		// 检查这个内存块是否是一个CEObject
		for (const auto& [obj_ptr, _] : Raw.RawRObjectHandles)
		{
			if (obj_ptr == ptr)
			{
				obj = static_cast<CEObject*>(obj_ptr);
				break;
			}
		}
		allocated_blocks.push_back({ ptr, size, obj });
	}

	// 收集所有线程本地缓存中已分配的块
	{
		std::lock_guard<std::mutex> cache_lock(Raw.cache_mutex);
		for (auto& [thread_id, cache] : Raw.thread_caches) {
			std::lock_guard<std::mutex> thread_lock(cache.local_mutex);
			for (const auto& [ptr, size] : cache.local_alloc_blocks) {
				CEObject* obj = nullptr;
				// 检查这个内存块是否是一个CEObject
				for (const auto& [obj_ptr, _] : Raw.RawRObjectHandles) {
					if (obj_ptr == ptr) {
						obj = static_cast<CEObject*>(obj_ptr);
						break;
					}
				}
				allocated_blocks.push_back({ ptr, size, obj });
			}

			// 清空本地缓存的空闲块，以便GC后统一管理
			cache.local_free_blocks.clear();
			cache.local_alloc_blocks.clear();
		}
	}

	// 2. 按地址排序内存块
	std::sort(allocated_blocks.begin(), allocated_blocks.end(),
		[](const MemoryBlock& a, const MemoryBlock& b) {
			return a.ptr < b.ptr;
		});

	// 3. 重新分配内存以减少碎片
	// 保存当前的内存状态
	void* old_buffer = Raw.RawMemoryBuffer;
	size_t old_buffer_size = Raw.RawMemoryBufferSize;

	// 创建一个临时缓冲区
	void* temp_buffer = ::malloc(old_buffer_size);
	if (temp_buffer == nullptr)
	{
		// 内存分配失败，无法进行GC
		return;
	}

	// 清空临时缓冲区
	memset(temp_buffer, 0, old_buffer_size);

	// 重置内存管理容器
	Raw.AllocBlockContainer.clear();
	Raw.FreeBlockContainer.clear();
	Raw.FreeRBlockContainer.clear();

	// 设置新的内存缓冲区
	Raw.RawMemoryBuffer = temp_buffer;

	// 当前写入位置
	size_t current_pos = 0;

	// 复制所有分配的块到新缓冲区
	for (const auto& block : allocated_blocks)
	{
		// 复制内存
		void* new_ptr = static_cast<void*>((char*)temp_buffer + current_pos);
		memcpy(new_ptr, block.ptr, block.size);

		// 更新对象引用
		if (block.object != nullptr)
		{
			// 更新映射
			CEHandle handle = Raw.RawRObjectHandles[block.object];
			Raw.RawObjectHandles[handle] = static_cast<CEObject*>(new_ptr);
			Raw.RawRObjectHandles.erase(block.object);
			Raw.RawRObjectHandles[static_cast<CEObject*>(new_ptr)] = handle;
		}

		// 更新AllocBlockContainer
		Raw.AllocBlockContainer[new_ptr] = block.size;

		// 更新位置
		current_pos += block.size;
	}

	// 添加剩余内存作为单个空闲块
	if (current_pos < old_buffer_size)
	{
		void* free_ptr = static_cast<void*>((char*)temp_buffer + current_pos);
		size_t free_size = old_buffer_size - current_pos;
		Raw.FreeBlockContainer.insert({ free_size, free_ptr });
		Raw.FreeRBlockContainer[free_ptr] = free_size;
	}

	// 释放旧缓冲区
	::free(old_buffer);
}

#pragma endregion

CEHandle GetCEHandle(_In_opt_ CEObject* ptr)
{
	if (ptr == nullptr)
		return -1;
	if (Raw.Contains(ptr))
		return Raw.At(ptr);
	return -1;
}
_Ret_maybenull_ CEObject* GetCEObject(CEHandle handle)
{
	if (Raw.Contains(handle))
		return Raw.At(handle);
	return nullptr;
}

_Ret_notnull_ const char* GetName(CEHandle handle)
{
	auto obj = GetCEObject(handle);
	if (!obj) {
		static const char empty_name[1] = { 0 };
		return empty_name;
	}
	return obj->GetName();
}

void SetName(CEHandle handle, _In_ const char* name)
{
	auto obj = GetCEObject(handle);
	if (obj) {
		obj->SetName(name);
	}
}

void* CEObject::operator new(size_t size)
{
	void* ptr = Raw.MemoryAlloc(size);
	if (ptr == nullptr)
		throw std::bad_alloc();
	return ptr;
}

void CEObject::operator delete(void* ptr, size_t size)
{
	Raw.FreeMemory(ptr);
}
