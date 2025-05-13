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

#define CEKernel ConventionEngine::Kernel::

namespace ConventionEngine::Kernel
{
	constexpr const char* const CE_VERSION = "0.1.0";
	constexpr const char* const CE_BUILD_DATE = __DATE__;
	constexpr const char* const CE_BUILD_TIME = __TIME__;
	constexpr const char* const CE_BUILD_COMPILER = __VERSION__;
	constexpr const char* const CE_BUILD_COMPILER_ID = __COMPILER_ID__;
	constexpr const char* const CE_BUILD_COMPILER_VERSION = __COMPILER_VERSION__;
	constexpr const char* const CE_BUILD_COMPILER_NAME = __COMPILER_NAME__;

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
	CEKernel CEHandle GetHandle(_In_opt_ void* ptr);

	/**
	 * @brief 通过句柄获取对象指针
	 *
	 * @param handle 对象句柄
	 * @return 对象指针，失败时可能为空
	 */
	_Ret_maybenull_ void* GetPtr(CEKernel CEHandle handle);

	/**
	 * @brief 获取对象名称
	 *
	 * @param handle 对象句柄
	 * @return 对象名称字符串，永不为空
	 */
	_Ret_notnull_ const char* GetName(CEKernel CEHandle handle);

	/**
	 * @brief 设置对象名称
	 *
	 * @param handle 对象句柄
	 * @param name 新名称
	 */
	void SetName(CEKernel CEHandle handle, _In_ const char* name);
}

namespace ConventionEngine::Kernel
{
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
		_NODISCARD _Ty* CreatePtr(Args&&... args)
		{
			_Ty* ptr = this->allocate(sizeof(_Ty));
			this->construct(ptr, std::forward<Args>(args)...);
			return ptr;
		}

		void DestoryPtr(_In_ _Ty* ptr)
		{
			this->destroy(ptr);
			this->deallocate(ptr, sizeof(_Ty));
		}

		void DestoryPtr(_Inout_ _Ty*& ptr)
		{
			this->destroy(ptr);
			this->deallocate(ptr, sizeof(_Ty));
			ptr = nullptr;
		}

		void DestoryPtr(_Inout_ _Ty** ptr)
		{
			this->destroy(*ptr);
			this->deallocate(*ptr, sizeof(_Ty));
			*ptr = nullptr;
		}
	};

	/**
	 * @class CEPtr
	 * @brief ConventionEngine的智能指针类
	 *
	 * 此类用于管理ConventionEngine对象的生命周期，
	 * 提供引用计数和内存管理功能。
	 *
	 * @tparam _Ty 指向的对象类型
	 */
	template<typename _Ty>
	class CEPtr final : public any_class
	{
		template<typename _OtherTy>
		friend class CEPtr;
	public:
		/*static_assert(std::is_base_of_v<CEObject, _Ty>, "The Convention Engine Ptr must use a subclass that "
			"inherits from CEObject");*/
	private:
		using handle_instance = instance<CEHandle, true>;
		handle_instance m_instance;
		void* operator new(size_t size) = delete;
		explicit CEPtr(handle_instance handle) noexcept
			: m_instance(handle) {}
	public:
		CEPtr()
			: m_instance(new CEHandle(GetHandle(CEAllocator<_Ty>().CreatePtr()))) {}
		template<typename _Other>
		CEPtr(const CEPtr<_Other>& other) noexcept 
			: m_instance(static_cast<handle_instance::_shared>(other.m_instance))
		{
			static_assert(std::is_base_of_v<_Ty, _Other>, "cast is invaild");
		}
		CEPtr(const CEPtr& other) noexcept 
			: m_instance(static_cast<handle_instance::_shared>(other.m_instance)) {}
		CEPtr(CEPtr&& other) noexcept 
			: m_instance(std::move(other.m_instance)) {}
		/*template<typename... _Args>
		CEPtr(_Args&&... args) : CEPtr(CEAllocator<_Ty>().CreatePtr(std::forward<_Args>(args)...)) {}*/
		CEPtr& operator=(const CEPtr& other) noexcept
		{
			m_instance = other.m_instance;
			return *this;
		}
		CEPtr& operator=(CEPtr&& other) noexcept
		{
			m_instance = std::move(other.m_instance);
			return *this;
		}
		~CEPtr()
		{
			if (m_instance.use_count() <= 1)
			{
				CEAllocator<_Ty>().DestoryPtr(get());
			}
		}

		_Ty* get() const
		{
			if (m_instance.is_empty())
				return nullptr;
			return static_cast<_Ty*>(GetPtr(*m_instance));
		}

		operator _Ty* () const
		{
			return get();
		}

		operator _Ty& () const
		{
			return *get();
		}

		_Ty* operator->() const
		{
			return get();
		}
		_Ty* operator*() const
		{
			return get();
		}

		template<typename _Other>
		CEPtr<_Other> cast()
		{
			static_assert(std::is_base_of_v<_Ty, _Other> || std::is_base_of_v<_Other, _Ty>, "cast is invaild");
			return CEPtr<_Other>(this->m_instance);
		}
	private:
	};

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
		virtual CEPtr<CEObject> CreateObject() const abstract;

		/**
		 * @brief 将当前对象的值克隆到目标对象
		 *
		 * 派生类应重写此方法以复制所有相关成员变量
		 *
		 * @param target 目标对象指针
		 */
		virtual void CloneValuesTo(_In_ CEPtr<CEObject> target) const;

		template<typename _Ty>
		friend class CEPtr;
		template<typename _Ty>
		friend class CEAllocator;

		/**
		 * @brief 重载new运算符，使用内存池分配内存
		 *
		 * @param size 请求分配的内存大小
		 * @return 分配的内存块指针
		 */
		void* operator new(size_t size);
		CEObject();
	public:
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
		CEPtr<CEObject> Instantiate() const;

		/**
		 * @brief 将对象标记为已修改
		 */
		void SetDirty();
	};
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
	/*class Scene :public CEObject
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

	};*/
}

#endif // !__FILE_CONVENTION_ENGINE_INTERNAL
