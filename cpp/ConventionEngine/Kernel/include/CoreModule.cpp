#include "ConventionEngine/Kernel/CoreModule.h"
#include <mutex>
#include <thread>
#include <unordered_map>
#include <shared_mutex>

using namespace ConventionEngine::Kernel;

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
            | 更新对象句柄映射                 |
            | 通过哈希表快速查找               |
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

class RawT :public any_class
{
private:
	// 私有构造函数，防止外部创建实例
	RawT() : handle_tail(0), RawMemoryBuffer(nullptr), RawMemoryBufferSize(0) {}

	// 删除复制构造和赋值操作符
	RawT(const RawT&) = delete;
	RawT& operator=(const RawT&) = delete;

	// 单例实例指针
	static RawT* instance;
	// 保护单例初始化的互斥锁
	static std::mutex singleton_mutex;

	CEHandle handle_tail = 0;

	std::unordered_map<CEHandle, void*> RawObjectHandles = {};
	std::unordered_map<void*, CEHandle> RawRObjectHandles = {};

	void* RawMemoryBuffer = nullptr;
	size_t RawMemoryBufferSize = 0;

	std::multimap<size_t, void*> FreeBlockContainer = {};
	std::map<void*, size_t> FreeRBlockContainer = {};
	std::map<void*, size_t> AllocBlockContainer = {};

	// 内存统计缓存
	mutable struct {
		size_t totalFreeMemory = 0;         // 总空闲内存
		size_t largestFreeBlock = 0;        // 最大空闲块
		size_t allocatedMemory = 0;         // 已分配内存
		bool isValid = false;               // 缓存是否有效
		std::shared_mutex cacheMutex;       // 保护缓存的互斥锁
	} memoryStatCache;

	// 线程本地缓存 - 每个线程都有自己的内存池
	struct ThreadCache
	{
		std::multimap<size_t, void*> local_free_blocks; // 本地空闲块
		std::map<void*, size_t> local_alloc_blocks;    // 本地已分配块
		std::mutex local_mutex;                        // 本地锁
	};

	// 线程本地缓存映射表
	std::unordered_map<std::thread::id, ThreadCache> thread_caches;
	std::mutex cache_mutex; // 保护thread_caches映射表

public:
	// 获取单例实例的静态方法
	static RawT& GetInstance() {
		// 双重检查锁定模式 (DCLP)
		if (instance == nullptr) {
			std::lock_guard<std::mutex> lock(singleton_mutex);
			if (instance == nullptr) {
				// 创建新实例
				instance = new RawT();
			}
		}
		return *instance;
	}

	// 释放单例实例的静态方法（仅在程序结束时调用）
	static void ReleaseInstance() {
		std::lock_guard<std::mutex> lock(singleton_mutex);
		if (instance != nullptr) {
			delete instance;
			instance = nullptr;
		}
	}

	// 获取当前线程的缓存
	ThreadCache& GetThreadCache() {
		std::thread::id this_id = std::this_thread::get_id();
		std::lock_guard<std::mutex> lock(cache_mutex);
		return thread_caches[this_id];
	}

	// 内部方法：重新计算内存统计数据
	void RecalculateMemoryStats() const {
		// 在调用此函数之前应该已经获取了适当的锁

		// 清空缓存数据
		memoryStatCache.totalFreeMemory = 0;
		memoryStatCache.largestFreeBlock = 0;
		memoryStatCache.allocatedMemory = 0;

		// 计算总空闲内存和最大空闲块
		for (const auto& [size, _] : FreeBlockContainer)
		{
			memoryStatCache.totalFreeMemory += size;
			memoryStatCache.largestFreeBlock = std::max(memoryStatCache.largestFreeBlock, size);
		}

		// 计算全局已分配内存
		for (const auto& [_, size] : AllocBlockContainer)
		{
			memoryStatCache.allocatedMemory += size;
		}

		// 统计所有线程本地缓存 - 避免死锁的安全方式
		// 创建线程本地缓存的快照，避免在持有全局锁的情况下再获取线程本地锁
		struct ThreadCacheSnapshot {
			size_t totalFreeMemory = 0;
			size_t largestFreeBlock = 0;
			size_t allocatedMemory = 0;
		};
		std::vector<ThreadCacheSnapshot> cacheSnapshots;

		{
			// 只使用cache_mutex锁，避免嵌套锁导致的死锁
			std::lock_guard<std::mutex> cache_lock(const_cast<std::mutex&>(cache_mutex));

			// 为每个线程缓存分配一个快照
			cacheSnapshots.resize(thread_caches.size());
			size_t index = 0;

			for (const auto& [thread_id, cache] : thread_caches) {
				auto& snapshot = cacheSnapshots[index++];

				// 尝试获取线程本地锁，但使用try_lock避免死锁
				std::unique_lock<std::mutex> thread_lock(const_cast<std::mutex&>(cache.local_mutex), std::try_to_lock);

				if (thread_lock.owns_lock()) {
					// 成功获取锁，计算这个线程的缓存统计
					for (const auto& [size, _] : cache.local_free_blocks) {
						snapshot.totalFreeMemory += size;
						snapshot.largestFreeBlock = std::max(snapshot.largestFreeBlock, size);
					}

					for (const auto& [_, size] : cache.local_alloc_blocks) {
						snapshot.allocatedMemory += size;
					}
				} else {
					// 无法获取锁，使用近似值
					// 这是一个权衡：我们宁可返回不完全准确的统计数据，也不要冒死锁的风险
					// 由于缓存本来就是无效的（isValid=false才会执行此函数），所以近似值是可接受的

					// 不做任何操作，保持snapshot的默认值0
					// 在下一次统计时会重新计算
				}
			}
		}

		// 合并所有线程缓存的统计数据
		for (const auto& snapshot : cacheSnapshots) {
			memoryStatCache.totalFreeMemory += snapshot.totalFreeMemory;
			memoryStatCache.largestFreeBlock = std::max(memoryStatCache.largestFreeBlock, snapshot.largestFreeBlock);
			memoryStatCache.allocatedMemory += snapshot.allocatedMemory;
		}

		// 标记缓存为有效
		memoryStatCache.isValid = true;
	}

	// 内部方法：使缓存无效
	void InvalidateCache()
	{
		std::lock_guard<std::shared_mutex> lock(memoryStatCache.cacheMutex);
		memoryStatCache.isValid = false;
	}

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

	// 友元函数声明
	friend void InitConventionEngine(size_t memory_size);
	friend void ClearConventionEngine();
	friend void QuitConventionEngine();
	friend void GC_ConventionEngine();

	_Ret_notnull_ void* MemoryAlloc(size_t size)
	{
		// 确保请求的内存大小不为0
		if (size == 0) size = sizeof(char);

		// 使缓存失效，在获取任何其他锁之前
		InvalidateCache();

		// 尝试从线程本地缓存分配
#ifndef __RawT_Disable_Thread_Cache
		ThreadCache* thread_cache_ptr = nullptr;

		// 首先获取线程缓存对象的指针，使用局部范围的锁
		{
			std::lock_guard<std::mutex> cache_lock(cache_mutex);
			std::thread::id this_id = std::this_thread::get_id();
			thread_cache_ptr = &thread_caches[this_id];
		}

		// 使用线程本地缓存，不再持有全局缓存锁
		{
			std::lock_guard<std::mutex> local_lock(thread_cache_ptr->local_mutex);

			// 检查线程本地缓存是否有足够大的块
			auto local_it = thread_cache_ptr->local_free_blocks.lower_bound(size);
			if (local_it != thread_cache_ptr->local_free_blocks.end()) {
				auto&& [blocksize, ptr] = *local_it;

				// 从线程本地缓存中移除此块
				thread_cache_ptr->local_free_blocks.erase(local_it);

				// 如果块比需要的大，将剩余部分添加回本地缓存
				auto right_block_size = blocksize - size;
				if (right_block_size > sizeof(void*)) {
					void* new_free_ptr = static_cast<void*>((char*)ptr + size);
					thread_cache_ptr->local_free_blocks.insert({ right_block_size, new_free_ptr });
				}

				// 添加到本地已分配块
				thread_cache_ptr->local_alloc_blocks[ptr] = size;
				return ptr;
			}
		}
#endif

		// 线程本地缓存中没有合适的块，需要从全局池分配
		std::lock_guard<std::mutex> alloc_lock(RawT_Tools::allocation_mutex);

		// 如果没有足够的内存块，尝试GC
		if (FreeBlockContainer.empty() || FreeBlockContainer.rbegin()->first < size)
		{
			// 注意：GC_ConventionEngine会获取内部锁，但这是安全的因为我们遵循固定的锁顺序
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
				};

				std::vector<MemoryBlock> allocated_blocks;

				// 收集全局已分配的内存块
				for (const auto& [ptr, size] : AllocBlockContainer)
				{
					allocated_blocks.push_back({ ptr, size });
				}

				// 收集所有线程本地缓存中已分配的块
				// 为避免死锁，我们使用try_lock并跳过无法锁定的线程
				{
					std::lock_guard<std::mutex> cache_lock(cache_mutex);
					for (auto& [thread_id, cache] : thread_caches) {
						std::unique_lock<std::mutex> thread_lock(cache.local_mutex, std::try_to_lock);
						if (thread_lock.owns_lock()) {
							for (const auto& [ptr, size] : cache.local_alloc_blocks) {
								allocated_blocks.push_back({ ptr, size });
							}
						}
						// 如果不能锁定，跳过此线程的块
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
						std::unique_lock<std::mutex> thread_lock(cache.local_mutex, std::try_to_lock);
						if (thread_lock.owns_lock()) {
							cache.local_free_blocks.clear();
							cache.local_alloc_blocks.clear();
						}
						// 如果不能锁定，跳过此线程的清理
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
					memmove(new_ptr, block.ptr, block.size);

					// 更新对象引用（如果在RawRObjectHandles中）
					auto it = RawRObjectHandles.find(block.ptr);
					if (it != RawRObjectHandles.end())
					{
						CEHandle handle = it->second;
						RawObjectHandles[handle] = new_ptr;
						RawRObjectHandles.erase(it);
						RawRObjectHandles[new_ptr] = handle;
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
			auto [blocksize, ptr] = *it;
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

#ifndef __RawT_Disable_Thread_Cache
			// 重新获取线程本地锁，将结果添加到线程本地已分配块容器
			std::lock_guard<std::mutex> local_lock(thread_cache_ptr->local_mutex);
			thread_cache_ptr->local_alloc_blocks[result] = size;
#else
			// 如果禁用了线程本地缓存，直接添加到全局已分配块容器
			AllocBlockContainer[result] = size;
#endif
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

		// 使缓存失效，在获取任何其他锁之前
		InvalidateCache();

#ifndef __RawT_Disable_Thread_Cache
		// 首先获取线程缓存指针，使用局部范围的锁
		ThreadCache* thread_cache_ptr = nullptr;
		{
			std::lock_guard<std::mutex> cache_lock(cache_mutex);
			std::thread::id this_id = std::this_thread::get_id();
			thread_cache_ptr = &thread_caches[this_id];
		}

		// 检查是否在线程本地缓存中
		{
			std::unique_lock<std::mutex> local_lock(thread_cache_ptr->local_mutex);
			auto local_it = thread_cache_ptr->local_alloc_blocks.find(ptr);
			if (local_it != thread_cache_ptr->local_alloc_blocks.end())
			{
				// 在本地缓存中找到了
				size_t blocksize = local_it->second;
				thread_cache_ptr->local_alloc_blocks.erase(local_it);

				// 添加到本地空闲块
				thread_cache_ptr->local_free_blocks.insert({ blocksize, ptr });

				// 如果本地空闲块太多，可以考虑返回一部分给全局池
				const size_t MAX_LOCAL_FREE_BLOCKS = __RawT_Max_Thread_Cache_Free_Blocks; // 可调整的阈值
				if (thread_cache_ptr->local_free_blocks.size() > MAX_LOCAL_FREE_BLOCKS) {
					// 将最大的块返回给全局池 - 这里需要获取全局分配锁
					auto largest_it = thread_cache_ptr->local_free_blocks.rbegin();
					size_t largest_size = largest_it->first;
					void* largest_ptr = largest_it->second;

					// 从线程本地移除最大块
					thread_cache_ptr->local_free_blocks.erase(--largest_it.base());

					// 先释放线程本地锁，再获取全局锁，避免死锁
					local_lock.unlock();

					std::lock_guard<std::mutex> alloc_lock(RawT_Tools::allocation_mutex);
					FreeBlockContainer.insert({ largest_size, largest_ptr });
					FreeRBlockContainer[largest_ptr] = largest_size;
				}
				return;
			}
		}
#endif

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

	size_t GetTotalMemory() const
	{
		// 内存池总大小是不变的，除非在扩展时，而扩展已经有全局锁保护
		std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
		return RawMemoryBufferSize;
	}

	// 获取当前内存使用情况的方法
	size_t GetTotalFreeMemory() const
	{
		// 首先检查缓存是否有效
		{
			std::shared_lock<std::shared_mutex> cache_lock(memoryStatCache.cacheMutex);
			if (memoryStatCache.isValid) {
				return memoryStatCache.totalFreeMemory;
			}
		}

		// 缓存无效时的优化锁策略:
		// 1. 先获取计算所需的所有状态数据快照（加锁）
		// 2. 解锁
		// 3. 使用快照进行计算（无锁）
		// 4. 更新缓存（加锁）

		// 我们需要收集的数据
		struct MemoryStateSnapshot {
			std::multimap<size_t, void*> freeBlocks;
			std::map<void*, size_t> allocBlocks;
			std::vector<std::pair<std::multimap<size_t, void*>, std::map<void*, size_t>>> threadCaches;
		};

		MemoryStateSnapshot snapshot;

		// 步骤1：获取全局数据快照（持有全局锁）
		{
			std::shared_lock<std::shared_mutex> lock(RawT_Tools::global_mutex);
			snapshot.freeBlocks = FreeBlockContainer;
			snapshot.allocBlocks = AllocBlockContainer;
		}

		// 步骤1.2：获取线程缓存快照（持有缓存锁但不需要全局锁）
		{
			std::lock_guard<std::mutex> cache_lock(const_cast<std::mutex&>(cache_mutex));

			for (const auto& [thread_id, cache] : thread_caches) {
				// 尝试获取线程本地锁，但使用try_lock避免死锁
				std::unique_lock<std::mutex> thread_lock(const_cast<std::mutex&>(cache.local_mutex), std::try_to_lock);

				if (thread_lock.owns_lock()) {
					// 成功获取锁，添加这个线程的缓存数据到快照
					snapshot.threadCaches.push_back({cache.local_free_blocks, cache.local_alloc_blocks});
				}
				// 如果无法获取锁，跳过此线程的数据
			}
		}

		// 步骤2：使用快照计算结果（无锁状态）
		size_t totalFree = 0;
		size_t largestBlock = 0;
		size_t totalAllocated = 0;

		// 计算全局空闲内存
		for (const auto& [size, _] : snapshot.freeBlocks) {
			totalFree += size;
			largestBlock = std::max(largestBlock, size);
		}

		// 计算全局已分配内存
		for (const auto& [_, size] : snapshot.allocBlocks) {
			totalAllocated += size;
		}

		// 计算线程本地缓存数据
		for (const auto& [freeBlocks, allocBlocks] : snapshot.threadCaches) {
			for (const auto& [size, _] : freeBlocks) {
				totalFree += size;
				largestBlock = std::max(largestBlock, size);
			}

			for (const auto& [_, size] : allocBlocks) {
				totalAllocated += size;
			}
		}

		// 步骤3：更新缓存（持有缓存锁）
		{
			std::lock_guard<std::shared_mutex> cache_lock(memoryStatCache.cacheMutex);
			memoryStatCache.totalFreeMemory = totalFree;
			memoryStatCache.largestFreeBlock = largestBlock;
			memoryStatCache.allocatedMemory = totalAllocated;
			memoryStatCache.isValid = true;

			return memoryStatCache.totalFreeMemory;
		}
	}

	size_t GetLargestFreeBlock() const
	{
		// 首先检查缓存是否有效
		{
			std::shared_lock<std::shared_mutex> cache_lock(memoryStatCache.cacheMutex);
			if (memoryStatCache.isValid) {
				return memoryStatCache.largestFreeBlock;
			}
		}

		// 缓存无效，重用GetTotalFreeMemory的计算结果
		// 这会同时计算所有三个值，并在内部更新缓存
		GetTotalFreeMemory();

		// 返回更新后的缓存值
		std::shared_lock<std::shared_mutex> cache_lock(memoryStatCache.cacheMutex);
		return memoryStatCache.largestFreeBlock;
	}

	size_t GetAllocatedMemory() const
	{
		// 首先检查缓存是否有效
		{
			std::shared_lock<std::shared_mutex> cache_lock(memoryStatCache.cacheMutex);
			if (memoryStatCache.isValid) {
				return memoryStatCache.allocatedMemory;
			}
		}

		// 缓存无效，重用GetTotalFreeMemory的计算结果
		// 这会同时计算所有三个值，并在内部更新缓存
		GetTotalFreeMemory();

		// 返回更新后的缓存值
		std::shared_lock<std::shared_mutex> cache_lock(memoryStatCache.cacheMutex);
		return memoryStatCache.allocatedMemory;
	}
};

// 初始化静态成员
RawT* RawT::instance = nullptr;
std::mutex RawT::singleton_mutex;

// 修改全局函数以使用单例
_Ret_notnull_ void* ConventionEngineMemoryAlloc(size_t size)
{
	return RawT::GetInstance().MemoryAlloc(size);
}

void ConventionEngineFreeMemory(_In_ void* ptr)
{
	RawT::GetInstance().FreeMemory(ptr);
}

void InitConventionEngine(size_t memory_size)
{
	std::lock_guard<std::shared_mutex> lock(RawT_Tools::global_mutex);

	RawT& raw = RawT::GetInstance();
	raw.RawMemoryBufferSize = memory_size;
	raw.RawMemoryBuffer = ::malloc(sizeof(char) * raw.RawMemoryBufferSize);
	if (raw.RawMemoryBuffer == nullptr)
	{
		// 处理内存分配失败
		raw.RawMemoryBufferSize = 0;
		return;
	}

	// 初始化时整个内存块都是空闲的
	raw.FreeBlockContainer.insert({ raw.RawMemoryBufferSize, raw.RawMemoryBuffer });
	raw.FreeRBlockContainer[raw.RawMemoryBuffer] = raw.RawMemoryBufferSize;

	// 清空其他容器
	raw.AllocBlockContainer.clear();
	raw.RawObjectHandles.clear();
	raw.RawRObjectHandles.clear();

	// 清空线程缓存
	std::lock_guard<std::mutex> cache_lock(raw.cache_mutex);
	raw.thread_caches.clear();

	// 初始化内存统计缓存
	{
		std::lock_guard<std::shared_mutex> cache_lock(raw.memoryStatCache.cacheMutex);
		raw.memoryStatCache.totalFreeMemory = raw.RawMemoryBufferSize;
		raw.memoryStatCache.largestFreeBlock = raw.RawMemoryBufferSize;
		raw.memoryStatCache.allocatedMemory = 0;
		raw.memoryStatCache.isValid = true;
	}
}

void ClearConventionEngine()
{
	std::lock_guard<std::shared_mutex> lock(RawT_Tools::global_mutex);

	RawT& raw = RawT::GetInstance();

	// 删除所有对象
	for (auto&& [_, ptr] : raw.RawObjectHandles)
	{
		delete ptr;
	}

	// 清空所有容器
	raw.RawObjectHandles.clear();
	raw.RawRObjectHandles.clear();
	raw.AllocBlockContainer.clear();
	raw.FreeBlockContainer.clear();
	raw.FreeRBlockContainer.clear();

	// 清空线程缓存
	std::lock_guard<std::mutex> cache_lock(raw.cache_mutex);
	raw.thread_caches.clear();

	// 如果内存缓冲区存在，重置内存池状态
	if (raw.RawMemoryBuffer != nullptr) {
		memset(raw.RawMemoryBuffer, 0, raw.RawMemoryBufferSize);
		raw.FreeBlockContainer.insert({ raw.RawMemoryBufferSize, raw.RawMemoryBuffer });
		raw.FreeRBlockContainer[raw.RawMemoryBuffer] = raw.RawMemoryBufferSize;
	}

	// 更新内存统计缓存
	{
		std::lock_guard<std::shared_mutex> cache_lock(raw.memoryStatCache.cacheMutex);
		raw.memoryStatCache.totalFreeMemory = raw.RawMemoryBufferSize;
		raw.memoryStatCache.largestFreeBlock = raw.RawMemoryBufferSize;
		raw.memoryStatCache.allocatedMemory = 0;
		raw.memoryStatCache.isValid = true;
	}
}

void QuitConventionEngine()
{
	// 防止重复调用引起的问题
	static bool already_quit = false;
	if (already_quit) {
		// 已经退出过，不需要再次清理
		return;
	}

	// 获取单例的引用
	RawT& raw = RawT::GetInstance();

	if (raw.RawMemoryBuffer == nullptr) {
		// 内存池为空，不需要清理
		return;
	}

	std::lock_guard<std::shared_mutex> lock(RawT_Tools::global_mutex);

	// 确保所有对象都被删除
	for (auto&& [_, ptr] : raw.RawObjectHandles)
	{
		if (ptr != nullptr) {
			delete ptr;
		}
	}

	// 清空所有容器 - 使用安全的方式
	raw.RawObjectHandles.clear();
	raw.RawRObjectHandles.clear();

	// 安全清理AllocBlockContainer（问题发生位置）
	try {
		raw.AllocBlockContainer.clear();
	}
	catch (...) {
		// 捕获任何异常但继续清理其他资源
	}

	try {
		raw.FreeBlockContainer.clear();
		raw.FreeRBlockContainer.clear();
	}
	catch (...) {
		// 捕获任何异常但继续清理其他资源
	}

	// 清空线程缓存
	try {
		std::lock_guard<std::mutex> cache_lock(raw.cache_mutex);
		raw.thread_caches.clear();
	}
	catch (...) {
		// 继续执行
	}

	// 释放内存池
	if (raw.RawMemoryBuffer != nullptr) {
		::free(raw.RawMemoryBuffer);
		raw.RawMemoryBuffer = nullptr;
		raw.RawMemoryBufferSize = 0;
	}

	// 重置内存统计缓存
	try {
		std::lock_guard<std::shared_mutex> cache_lock(raw.memoryStatCache.cacheMutex);
		raw.memoryStatCache.totalFreeMemory = 0;
		raw.memoryStatCache.largestFreeBlock = 0;
		raw.memoryStatCache.allocatedMemory = 0;
		raw.memoryStatCache.isValid = false;
	}
	catch (...) {
		// 继续执行
	}

	// 标记为已退出
	already_quit = true;
}

void GC_ConventionEngine()
{
	// 获取单例的引用
	RawT& raw = RawT::GetInstance();

	// 首先使缓存无效，在获取任何其他锁之前
	{
		std::lock_guard<std::shared_mutex> cache_lock(raw.memoryStatCache.cacheMutex);
		raw.memoryStatCache.isValid = false;
	}

	// 使用全局互斥锁保护GC过程
	std::lock_guard<std::shared_mutex> lock(RawT_Tools::global_mutex);

	// 这个函数不会自动回收CEObject，而是通过整理内存来减少碎片

	// 1. 收集所有已分配的内存块
	struct MemoryBlock {
		void* ptr;
		size_t size;
	};

	std::vector<MemoryBlock> allocated_blocks;

	// 收集全局已分配的内存块
	for (const auto& [ptr, size] : raw.AllocBlockContainer)
	{
		allocated_blocks.push_back({ ptr, size });
	}

	// 收集所有线程本地缓存中已分配的块 - 使用安全的锁策略
	{
		// 第一步：获取所有线程ID
		std::vector<std::thread::id> thread_ids;
		{
			std::lock_guard<std::mutex> cache_lock(raw.cache_mutex);
			for (const auto& [thread_id, _] : raw.thread_caches) {
				thread_ids.push_back(thread_id);
			}
		}

		// 第二步：对每个线程，尝试获取其线程本地锁（使用try_lock避免死锁）
		for (const auto& thread_id : thread_ids) {
			decltype(raw.thread_caches.find(thread_id)->second)* cache_ptr = nullptr;

			// 获取线程缓存指针
			{
				std::lock_guard<std::mutex> cache_lock(raw.cache_mutex);
				auto it = raw.thread_caches.find(thread_id);
				if (it != raw.thread_caches.end()) {
					cache_ptr = &(it->second);
				}
			}

			// 如果找到缓存指针，尝试获取其锁
			if (cache_ptr) {
				std::unique_lock<std::mutex> thread_lock(cache_ptr->local_mutex, std::try_to_lock);
				if (thread_lock.owns_lock()) {
					// 成功获取锁，收集已分配块并清空缓存
					for (const auto& [ptr, size] : cache_ptr->local_alloc_blocks) {
						allocated_blocks.push_back({ ptr, size });
					}

					// 清空本地缓存的空闲块，以便GC后统一管理
					cache_ptr->local_free_blocks.clear();
					cache_ptr->local_alloc_blocks.clear();
				}
				// 如果无法获取锁，跳过此线程的数据
			}
		}
	}

	// 如果没有收集到任何已分配的块，GC没有必要执行
	if (allocated_blocks.empty()) {
		// 将整个内存池作为一个空闲块
		raw.FreeBlockContainer.clear();
		raw.FreeRBlockContainer.clear();
		raw.FreeBlockContainer.insert({ raw.RawMemoryBufferSize, raw.RawMemoryBuffer });
		raw.FreeRBlockContainer[raw.RawMemoryBuffer] = raw.RawMemoryBufferSize;
		return;
	}

	// 其余GC过程保持不变，但将Raw替换为raw
	// 2. 按地址排序内存块
	std::sort(allocated_blocks.begin(), allocated_blocks.end(),
		[](const MemoryBlock& a, const MemoryBlock& b) {
			return a.ptr < b.ptr;
		});

	// 3. 重新分配内存以减少碎片
	// 保存当前的内存状态
	void* old_buffer = raw.RawMemoryBuffer;
	size_t old_buffer_size = raw.RawMemoryBufferSize;

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
	raw.AllocBlockContainer.clear();
	raw.FreeBlockContainer.clear();
	raw.FreeRBlockContainer.clear();

	// 设置新的内存缓冲区
	raw.RawMemoryBuffer = temp_buffer;

	// 当前写入位置
	size_t current_pos = 0;

	// 复制所有分配的块到新缓冲区
	for (const auto& block : allocated_blocks)
	{
		// 复制内存
		void* new_ptr = static_cast<void*>((char*)temp_buffer + current_pos);
		memmove(new_ptr, block.ptr, block.size);

		// 更新对象引用（如果在RawRObjectHandles中）
		auto it = raw.RawRObjectHandles.find(block.ptr);
		if (it != raw.RawRObjectHandles.end())
		{
			CEHandle handle = it->second;
			raw.RawObjectHandles[handle] = new_ptr;
			raw.RawRObjectHandles.erase(it);
			raw.RawRObjectHandles[new_ptr] = handle;
		}

		// 更新AllocBlockContainer
		raw.AllocBlockContainer[new_ptr] = block.size;

		// 更新位置
		current_pos += block.size;
	}

	// 添加剩余内存作为单个空闲块
	if (current_pos < old_buffer_size)
	{
		void* free_ptr = static_cast<void*>((char*)temp_buffer + current_pos);
		size_t free_size = old_buffer_size - current_pos;
		raw.FreeBlockContainer.insert({ free_size, free_ptr });
		raw.FreeRBlockContainer[free_ptr] = free_size;
	}

	// 释放旧缓冲区
	::free(old_buffer);
}

size_t GetEngineTotalMemory()
{
	return RawT::GetInstance().GetTotalMemory();
}

size_t GetEngineTotalFreeMemory()
{
	return RawT::GetInstance().GetTotalFreeMemory();
}

size_t GetEngineLargestFreeBlock()
{
	return RawT::GetInstance().GetLargestFreeBlock();
}

size_t GetEngineAllocatedMemory()
{
	return RawT::GetInstance().GetAllocatedMemory();
}

CEHandle GetHandle(_In_opt_ void* ptr)
{
	if (ptr == nullptr)
		return -1;
	if (RawT::GetInstance().Contains(ptr))
		return RawT::GetInstance().At(ptr);
	return -1;
}

_Ret_maybenull_ void* GetPtr(CEHandle handle)
{
	if (RawT::GetInstance().Contains(handle))
		return RawT::GetInstance().At(handle);
	return nullptr;
}

_Ret_notnull_ const char* GetName(CEHandle handle)
{
	return static_cast<CEObject*>(GetPtr(handle))->GetName();
}

void SetName(CEHandle handle, _In_ const char* name)
{
	static_cast<CEObject*>(GetPtr(handle))->SetName(name);
}

void* CEObject::operator new(size_t size)
{
	/*void* ptr = Raw.MemoryAlloc(size);
	if (ptr == nullptr)
		throw std::bad_alloc();
	return ptr;*/
	return ::operator new(size);
}

void CEObject::operator delete(void* ptr, size_t size)
{
	//Raw.FreeMemory(ptr);
	::operator delete(ptr, size);
}

CEPtr<CEObject> CEObject::Instantiate() const
{
	auto result = CreateObject();
	CloneValuesTo(result);
	return result;
}

void CEObject::CloneValuesTo(_In_ CEPtr<CEObject> target) const
{
	target->SetName(this->GetName());
	target->SetDirty();
}

void CEObject::SetDirty()
{
	this->m_dirty = true;
}

#pragma region QuickCreateObject

//QuickCreateObject(Scene);
//QuickCreateObject(SceneObject)
//QuickCreateObject(Component);

#pragma endregion
