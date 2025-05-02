/**
 * @file CoreModule.h
 * @brief ConventionEngine内核的核心模块，提供内存管理和对象基础设施
 *
 * 本文件包含ConventionEngine的核心功能，包括内存分配器（CEAllocator）、
 * 基础对象类（CEObject）以及一系列使用自定义内存管理的容器类型。
 * 整个引擎的内存管理基于RawT内存池，提供高效的内存分配和回收机制，
 * 同时支持垃圾回收和碎片整理。
 */

#ifndef __FILE_CONVENTION_ENGINE_INTERNAL

#define __FILE_CONVENTION_ENGINE_INTERNAL

#include "Convention/Config.h"
#include "Convention/Instance.h"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <list>
#include <deque>
#include <set>
#include <unordered_set>

/**
 * @def __NameMaxLength
 * @brief 定义CEObject对象名称的最大长度
 *
 * 默认为256字符，可以在编译时通过定义此宏修改
 */
#ifndef __NameMaxLength
#define __NameMaxLength 256
#endif // !__NameMaxLength

/**
 * @def __RawT_Memory_K
 * @brief 内存池扩容系数
 *
 * 当内存池需要扩容时，新容量为当前容量乘以此系数
 * 默认值为1.5，可以在编译时通过定义此宏修改
 */
#ifndef __RawT_Memory_K
#define __RawT_Memory_K 1.5
#endif // !__RawT_Memory_K

/**
 * @def __RawT_Max_Thread_Cache_Free_Blocks
 * @brief 线程本地缓存的最大空闲块数量
 *
 * 当线程本地缓存中的空闲块超过此数量时，会将部分块返回给全局内存池
 * 默认值为64，可以在编译时通过定义此宏修改
 */
#ifndef __RawT_Max_Thread_Cache_Free_Blocks
#define __RawT_Max_Thread_Cache_Free_Blocks 64
#endif // !__RawT_Max_Thread_Cache_Free_Blocks

#define CE ConventionEngine::

namespace ConventionEngine
{
	/**
	 * @typedef CEError
	 * @brief 错误代码类型
	 */
	using CEError = uint64_t;

	/**
	 * @typedef CEHandle
	 * @brief 对象句柄类型，用于间接引用CEObject对象
	 */
	using CEHandle = int64_t;
}

/**
 * @brief 内存管理相关的C接口函数声明
 */
extern "C"
{
	/**
	 * @brief 从ConventionEngine内存池分配内存
	 *
	 * @param size 请求分配的内存大小（字节）
	 * @return 分配的内存块指针，永不为空
	 */
	_Ret_notnull_ void* ConventionEngineMemoryAlloc(size_t size);

	/**
	 * @brief 释放从ConventionEngine内存池分配的内存
	 *
	 * @param ptr 要释放的内存块指针
	 */
	void ConventionEngineFreeMemory(_In_ void* ptr);

	/**
	 * @brief 初始化ConventionEngine
	 *
	 * 分配内存池并初始化内部数据结构
	 *
	 * @param memory_size 内存池初始大小（字节）
	 */
	void InitConventionEngine(size_t memory_size);

	/**
	 * @brief 清理ConventionEngine
	 *
	 * 释放所有对象但保留内存池
	 */
	void ClearConventionEngine();

	/**
	 * @brief 退出ConventionEngine
	 *
	 * 释放所有对象和内存池
	 */
	void QuitConventionEngine();

	/**
	 * @brief 执行垃圾回收
	 *
	 * 整理内存碎片，优化内存使用
	 */
	void GC_ConventionEngine();

	size_t GetEngineTotalMemory();
	size_t GetEngineTotalFreeMemory();
	size_t GetEngineLargestFreeBlock();
	size_t GetEngineAllocatedMemory();

	/**
	 * @brief 获取对象句柄
	 *
	 * @param ptr 对象指针
	 * @return 对象句柄，失败时返回-1
	 */
	CE CEHandle GetHandle(_In_opt_ void* ptr);

	/**
	 * @brief 通过句柄获取对象指针
	 *
	 * @param handle 对象句柄
	 * @return 对象指针，失败时可能为空
	 */
	_Ret_maybenull_ void* GetPtr(CE CEHandle handle);

	/**
	 * @brief 获取对象名称
	 *
	 * @param handle 对象句柄
	 * @return 对象名称字符串，永不为空
	 */
	_Ret_notnull_ const char* GetName(CE CEHandle handle);

	/**
	 * @brief 设置对象名称
	 *
	 * @param handle 对象句柄
	 * @param name 新名称
	 */
	void SetName(CE CEHandle handle, _In_ const char* name);
}

namespace ConventionEngine
{
	/**
	 * @class CEObject
	 * @brief ConventionEngine的基础对象类
	 *
	 * 所有ConventionEngine管理的对象都应该继承自此类。
	 * 提供内存管理、命名、实例化等基本功能。
	 */
	class CEObject: public any_class
	{
	public:
		/**
		 * @brief 对象名称的最大长度常量
		 */
		constexpr static size_t name_max_length = __NameMaxLength;
	private:
		/**
		 * @brief 对象实例ID
		 */
		int m_instanceID = -1;

		/**
		 * @brief 对象名称
		 */
		char m_name[name_max_length] = { 0 };

		/**
		 * @brief 对象是否被修改标志
		 */
		bool m_dirty = false;

	protected:

		/**
		 * @brief 创建一个与当前对象相同类型的新对象
		 *
		 * 此方法用于多态对象的克隆，派生类应重写此方法以创建正确的对象类型
		 *
		 * @return 新创建的对象指针
		 */
		virtual CEObject* CreateObject() const;

		/**
		 * @brief 将当前对象的值克隆到目标对象
		 *
		 * 派生类应重写此方法以复制所有相关成员变量
		 *
		 * @param target 目标对象指针
		 */
		virtual void CloneValuesTo(_In_ CEObject* target) const;

	public:
		/**
		 * @brief 重载new运算符，使用内存池分配内存
		 *
		 * @param size 请求分配的内存大小
		 * @return 分配的内存块指针
		 */
		void* operator new(size_t size);
		/**
		 * @brief 重载delete运算符，使用内存池释放内存
		 *
		 * @param ptr 要释放的内存块指针
		 * @param size 内存块大小
		 */
		void operator delete(void* ptr, size_t size);

		/**
		 * @brief 虚析构函数，确保正确释放派生类资源
		 */
		virtual ~CEObject();

		/**
		 * @brief 获取对象名称
		 *
		 * @return 对象名称的字符串指针，永不为空
		 */
		_Notnull_ constexpr const char* GetName() const;

		/**
		 * @brief 设置对象名称
		 *
		 * @param name 新的对象名称
		 */
		void SetName(_In_ const char* name);

		/**
		 * @brief 创建当前对象的副本
		 *
		 * 使用CreateObject创建对象，然后使用CloneValuesTo复制值
		 *
		 * @return 新创建的对象副本指针
		 */
		CEObject* Instantiate() const;

		/**
		 * @brief 将对象标记为已修改
		 */
		void SetDirty();
	};

	/**
	 * @class CEAllocator
	 * @brief 自定义内存分配器，与STL容器兼容
	 *
	 * 此分配器使用ConventionEngine的内存池进行内存分配和释放，
	 * 可用于标准库容器，使其使用引擎的内存管理机制。
	 *
	 * @tparam _Ty 分配器管理的元素类型
	 */
	template <class _Ty>
	class CEAllocator: public any_class
	{
	public:
		static_assert(!std::is_const_v<_Ty>, "The C++ Standard forbids containers of const elements "
			"because allocator<const T> is ill-formed.");
		static_assert(!std::is_function_v<_Ty>, "The C++ Standard forbids allocators for function elements "
			"because of [allocator.requirements].");
		static_assert(!std::is_reference_v<_Ty>, "The C++ Standard forbids allocators for reference elements "
			"because of [allocator.requirements].");
		//static_assert(std::is_base_of_v<CEObject, _Ty>, "The Convention Engine must use a subclass that "
		//	"inherits from CEObject");

		template<typename _RightTy>
		constexpr bool operator==(const CEAllocator<_RightTy>& right) const
		{
			return std::is_same_v<_Ty, _RightTy>;
		}
		template<typename _RightTy>
		constexpr bool operator!=(const CEAllocator<_RightTy>& right) const
		{
			return !(*this == right);
		}

		/**
		 * @typedef _From_primary
		 * @brief 主要分配器类型
		 */
		using _From_primary = CEAllocator;

		/**
		 * @typedef value_type
		 * @brief 分配器管理的元素类型
		 */
		using value_type = _Ty;

		/**
		 * @typedef pointer
		 * @brief 指向元素的指针类型
		 */
		using pointer = _Ty*;

		/**
		 * @typedef const_pointer
		 * @brief 指向常量元素的指针类型
		 */
		using const_pointer = const _Ty*;

		/**
		 * @typedef reference
		 * @brief 元素的引用类型
		 */
		using reference = _Ty&;

		/**
		 * @typedef const_reference
		 * @brief 常量元素的引用类型
		 */
		using const_reference = const _Ty&;

		/**
		 * @typedef size_type
		 * @brief 表示大小的类型
		 */
		using size_type = size_t;

		/**
		 * @typedef difference_type
		 * @brief 表示指针差的类型
		 */
		using difference_type = ptrdiff_t;

		/**
		 * @typedef propagate_on_container_move_assignment
		 * @brief 容器移动赋值时是否传播分配器
		 */
		using propagate_on_container_move_assignment = std::true_type;

		/**
		 * @typedef is_always_equal
		 * @brief 分配器实例是否总是相等
		 */
		using is_always_equal = std::true_type;

		/**
		 * @struct rebind
		 * @brief 重新绑定分配器到另一种类型
		 *
		 * @tparam _Other 要绑定到的新类型
		 */
		template <class _Other>
		struct rebind {
			using other = CEAllocator<_Other>;
		};

		/**
		 * @brief 获取元素地址
		 *
		 * @param _Val 元素引用
		 * @return 元素地址
		 */
		_NODISCARD _Ty* address(_Ty& _Val) const noexcept {
			return std::addressof(_Val);
		}

		/**
		 * @brief 获取常量元素地址
		 *
		 * @param _Val 常量元素引用
		 * @return 常量元素地址
		 */
		_NODISCARD const _Ty* address(const _Ty& _Val) const noexcept {
			return std::addressof(_Val);
		}

		/**
		 * @brief 默认构造函数
		 */
		constexpr CEAllocator() noexcept {}

		/**
		 * @brief 复制构造函数
		 */
		constexpr CEAllocator(const CEAllocator&) noexcept = default;

		/**
		 * @brief 从其他类型分配器构造
		 *
		 * @tparam _Other 其他分配器的元素类型
		 */
		template <class _Other>
		constexpr CEAllocator(const CEAllocator<_Other>&) noexcept {}

		/**
		 * @brief 析构函数
		 */
		_CONSTEXPR20 ~CEAllocator() = default;

		/**
		 * @brief 赋值操作符
		 */
		_CONSTEXPR20 CEAllocator& operator=(const CEAllocator&) = default;

		/**
		 * @brief 释放内存
		 *
		 * @param _Ptr 要释放的内存指针
		 * @param _Count 元素数量
		 */
		_CONSTEXPR20 void deallocate(_In_ _Ty* const _Ptr, const size_t _Count)
		{
			_STL_ASSERT(_Ptr != nullptr || _Count == 0, "null pointer cannot point to a block of non-zero size");
			ConventionEngineFreeMemory(_Ptr);
		}

		/**
		 * @brief 分配内存
		 *
		 * @param _Count 要分配的元素数量
		 * @return 分配的内存指针
		 */
		_allocator_ret_ _Ty* allocate(const size_t _Count)
		{
			static_assert(sizeof(value_type) > 0, "value_type must be complete before calling allocate.");
			return static_cast<_Ty*>(ConventionEngineMemoryAlloc(_Count));
		}

#if _HAS_CXX23
		/**
		 * @brief C++23分配至少指定大小的内存
		 *
		 * @param _Count 要分配的最小元素数量
		 * @return 分配结果（指针和实际分配的元素数量）
		 */
		constexpr allocation_result<_Ty*> allocate_at_least(const size_t _Count)
		{
			return { allocate(_Count), _Count };
		}
#endif // _HAS_CXX23

		/**
		 * @brief 带提示的内存分配
		 *
		 * @param _Count 要分配的元素数量
		 * @param 提示指针（未使用）
		 * @return 分配的内存指针
		 */
		_allocator_ret_ _Ty* allocate(const size_t _Count, const void*)
		{
			return allocate(_Count);
		}

		/**
		 * @brief 构造对象
		 *
		 * @tparam _Objty 对象类型
		 * @tparam _Types 构造函数参数类型
		 * @param _Ptr 对象位置
		 * @param _Args 构造函数参数
		 */
		template <class _Objty, class... _Types>
		void construct(_Objty* const _Ptr, _Types&&... _Args)
		{
			::new (const_cast<void*>(static_cast<const volatile void*>(_Ptr))) _Objty(std::forward<_Types>(_Args)...);
		}

		/**
		 * @brief 销毁对象
		 *
		 * @tparam _Uty 对象类型
		 * @param _Ptr 对象指针
		 */
		template <class _Uty>
		void destroy(_Uty* const _Ptr)
		{
			_Ptr->~_Uty();
		}

		/**
		 * @brief 获取分配器可以分配的最大元素数量
		 *
		 * @return 最大元素数量
		 */
		_NODISCARD size_t max_size() const noexcept
		{
			return static_cast<size_t>(-1) / sizeof(_Ty);
		}

		template<typename... Args>
		_Ty* CreatePtr(Args&&... args)
		{
			_Ty* ptr = this->allocate(sizeof(_Ty));
			this->construct(ptr, std::forward<Args>(args)...);
			return ptr;
		}
	};

	template<typename _Ty, typename... Args>
	instance<_Ty> make_ce_instance(Args&&... args)
	{
		auto alloc = CEAllocator<_Ty>();
		_Ty* ptr = alloc.allocate(sizeof(_Ty));
		alloc.construct(ptr, std::forward<Args>(args)...);
		return make_instance<_Ty>(ptr);
	}

	template<typename _Ty>
	class CEInstance :public instance<_Ty, true>
	{
	private:
	public:
		using _Mybase = instance<_Ty, true>;
		using tag = _Ty;
		constexpr CEInstance(nullptr_t): _Mybase(nullptr) {}
		explicit CEInstance(_Ty*) = delete;
		template<typename... Args>
		CEInstance(Args&&... args) : _Mybase(CEAllocator<_Ty>().CreatePtr(std::forward<Args>(args)...)) {}
		CEInstance(const CEInstance& other) :_Mybase(other) {}
		CEInstance(CEInstance&& other) :_Mybase(std::move(other)) {}
		CEInstance& operator=(const CEInstance& other)
		{
			_Mybase::operator=(other);
			return *this;
		}
		CEInstance& operator=(CEInstance&& other)
		{
			_Mybase::operator=(std::move(other));
			return std::move(*this);
		}
	public:
		virtual ~CEInstance()
		{
			if (this->use_count() <= 1)
			{
				CEAllocator<_Ty> alloc;
				alloc.destroy(this->get());
				alloc.deallocate(this->get(), sizeof(_Ty));
			}
		}
	};

	/**
	 * @namespace Types
	 * @brief 提供使用CEAllocator的标准库容器类型别名
	 *
	 * 此命名空间定义了一系列使用CEAllocator的STL容器的别名，
	 * 使用这些容器可以自动利用ConventionEngine的内存管理机制。
	 */
	namespace Types
	{
		/**
		 * @typedef STDString
		 * @brief 标准库字符串类型别名
		 */
		using STDString = std::string;

		/**
		 * @typedef STDWString
		 * @brief 标准库宽字符串类型别名
		 */
		using STDWString = std::wstring;

		/**
		 * @typedef String
		 * @brief 使用CEAllocator的字符串类型
		 */
		using String = CEInstance<std::basic_string<char, std::char_traits<char>, CEAllocator<char>>>;

		/**
		 * @typedef WString
		 * @brief 使用CEAllocator的宽字符串类型
		 */
		using WString = CEInstance<std::basic_string<wchar_t, std::char_traits<wchar_t>, CEAllocator<wchar_t>>>;

		/**
		 * @typedef Vector
		 * @brief 使用CEAllocator的向量容器
		 *
		 * @tparam T 元素类型
		 */
		template<typename T>
		using Vector = CEInstance<std::vector<T, CEAllocator<T>>>;

		/**
		 * @typedef List
		 * @brief 使用CEAllocator的链表容器
		 *
		 * @tparam T 元素类型
		 */
		template<typename T>
		using List = CEInstance<std::list<T, CEAllocator<T>>>;

		/**
		 * @typedef Deque
		 * @brief 使用CEAllocator的双端队列容器
		 *
		 * @tparam T 元素类型
		 */
		template<typename T>
		using Deque = CEInstance<std::deque<T, CEAllocator<T>>>;

		/**
		 * @typedef Map
		 * @brief 使用CEAllocator的映射容器
		 *
		 * @tparam K 键类型
		 * @tparam V 值类型
		 * @tparam Compare 比较器类型
		 */
		template<typename K, typename V, typename Compare = std::less<K>>
		using Map = CEInstance<std::map<K, V, Compare, CEAllocator<std::pair<const K, V>>>>;

		/**
		 * @typedef UnorderedMap
		 * @brief 使用CEAllocator的无序映射容器
		 *
		 * @tparam K 键类型
		 * @tparam V 值类型
		 * @tparam Hash 哈希函数类型
		 * @tparam Eq 相等比较器类型
		 */
		template<typename K, typename V, typename Hash = std::hash<K>, typename Eq = std::equal_to<K>>
		using UnorderedMap = CEInstance<std::unordered_map<K, V, Hash, Eq, CEAllocator<std::pair<const K, V>>>>;

		/**
		 * @typedef Set
		 * @brief 使用CEAllocator的集合容器
		 *
		 * @tparam T 元素类型
		 * @tparam Compare 比较器类型
		 */
		template<typename T, typename Compare = std::less<T>>
		using Set = CEInstance<std::set<T, Compare, CEAllocator<T>>>;

		/**
		 * @typedef UnorderedSet
		 * @brief 使用CEAllocator的无序集合容器
		 *
		 * @tparam T 元素类型
		 * @tparam Hash 哈希函数类型
		 * @tparam Eq 相等比较器类型
		 */
		template<typename T, typename Hash = std::hash<T>, typename Eq = std::equal_to<T>>
		using UnorderedSet = CEInstance<std::unordered_set<T, Hash, Eq, CEAllocator<T>>>;
	}
}

/**
 * @brief 显式实例化常用类型的分配器
 *
 * 这些实例化声明确保编译器生成这些特定类型的分配器代码
 */
// 字符串类型
template class ConventionEngine::CEAllocator<char>;
template class ConventionEngine::CEAllocator<wchar_t>;

// 基本数据类型
template class ConventionEngine::CEAllocator<int>;
template class ConventionEngine::CEAllocator<float>;
template class ConventionEngine::CEAllocator<double>;
template class ConventionEngine::CEAllocator<long>;
template class ConventionEngine::CEAllocator<unsigned int>;
template class ConventionEngine::CEAllocator<unsigned long>;
template class ConventionEngine::CEAllocator<bool>;

// 常用STL容器对应的pair类型
template class ConventionEngine::CEAllocator<std::pair<const int, int>>;
template class ConventionEngine::CEAllocator<std::pair<const std::string, std::string>>;
template class ConventionEngine::CEAllocator<std::pair<const int, std::string>>;
template class ConventionEngine::CEAllocator<std::pair<const std::string, int>>;

/**
 * @def QuickCreateObject
 * @brief 快速定义CreateObject方法的宏
 *
 * @param name 类名
 */
#define QuickCreateObject(name) CEObject* name::CreateObject() const {return new name();}

namespace ConventionEngine
{
	class Scene :public CEObject
	{
	public:

	private:

	protected:
		virtual CEObject* CreateObject() const override;
	public:

	};

	class SceneObject : public CEObject
	{
	public:

	private:

	protected:
		virtual CEObject* CreateObject() const override;
	public:
	};

	class Component :public CEObject
	{
	public:

	private:

	protected:
		virtual CEObject* CreateObject() const override;
	public:

	};
}

#endif // !__FILE_CONVENTION_ENGINE_INTERNAL
