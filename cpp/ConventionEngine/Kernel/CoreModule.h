#ifndef __FILE_CONVENTION_ENGINE_INTERNAL

#define __FILE_CONVENTION_ENGINE_INTERNAL

#include "Convention/Config.h"
#include "Convention/Instance.h"

#ifndef __NameMaxLength
#define __NameMaxLength 256
#endif // !__NameMaxLength

#ifndef __RawT_Memory_K
#define __RawT_Memory_K 1.5
#endif // !__RawT_Memory_K

#ifndef __RawT_Max_Thread_Cache_Free_Blocks
#define __RawT_Max_Thread_Cache_Free_Blocks 64
#endif // !__RawT_Max_Thread_Cache_Free_Blocks

extern "C"
{
	_Ret_notnull_ void* ConventionEngineMemoryAlloc(size_t size);
	void ConventionEngineFreeMemory(_In_ void* ptr);
}

namespace ConventionEngine
{
	class CEObject
	{
	public:
		constexpr static size_t name_max_length = __NameMaxLength;
	private:
		int m_instanceID = -1;
		char m_name[name_max_length] = { 0 };

	protected:
		void* operator new(size_t size);

	public:
		void operator delete(void* ptr, size_t size);
		virtual ~CEObject();

		_Notnull_ constexpr const char* GetName() const;
		void SetName(_In_ const char* name);
	};

	template <class _Ty>
	class CEAllocator
	{
	public:
		static_assert(!std::is_const_v<_Ty>, "The C++ Standard forbids containers of const elements "
			"because allocator<const T> is ill-formed.");
		static_assert(!std::is_function_v<_Ty>, "The C++ Standard forbids allocators for function elements "
			"because of [allocator.requirements].");
		static_assert(!std::is_reference_v<_Ty>, "The C++ Standard forbids allocators for reference elements "
			"because of [allocator.requirements].");
		static_assert(std::is_base_of_v<CEObject, _Ty>, "The Convention Engine must use a subclass that "
			"inherits from CEObject");

		using _From_primary = CEAllocator;

		using value_type = _Ty;

		using pointer = _Ty*;
		using const_pointer = const _Ty*;

		using reference = _Ty&;
		using const_reference = const _Ty&;

		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using propagate_on_container_move_assignment = std::true_type;
		using is_always_equal = std::true_type;

		template <class _Other>
		struct rebind {
			using other = CEAllocator<_Other>;
		};

		_NODISCARD _Ty* address(_Ty& _Val) const noexcept {
			return std::addressof(_Val);
		}

		_NODISCARD const _Ty* address(const _Ty& _Val) const noexcept {
			return std::addressof(_Val);
		}

		constexpr CEAllocator() noexcept {}
		constexpr CEAllocator(const CEAllocator&) noexcept = default;
		template <class _Other>
		constexpr CEAllocator(const CEAllocator<_Other>&) noexcept {}
		_CONSTEXPR20 ~CEAllocator() = default;
		_CONSTEXPR20 CEAllocator& operator=(const CEAllocator&) = default;

		_CONSTEXPR20 void deallocate(_In_ _Ty* const _Ptr, const size_t _Count) 
		{
			_STL_ASSERT(_Ptr != nullptr || _Count == 0, "null pointer cannot point to a block of non-zero size");
			ConventionEngineFreeMemory(_Ptr);
		}

		_allocator_ret_ _Ty* allocate(const size_t _Count) 
		{
			static_assert(sizeof(value_type) > 0, "value_type must be complete before calling allocate.");
			return static_cast<_Ty*>(ConventionEngineMemoryAlloc(_Count));
		}

#if _HAS_CXX23
		constexpr allocation_result<_Ty*> allocate_at_least(const size_t _Count) 
		{
			return { allocate(_Count), _Count };
		}
#endif // _HAS_CXX23

		_allocator_ret_ _Ty* allocate(const size_t _Count, const void*) 
		{
			return allocate(_Count);
		}

		template <class _Objty, class... _Types>
		void construct(_Objty* const _Ptr, _Types&&... _Args) 
		{
			::new (const_cast<void*>(static_cast<const volatile void*>(_Ptr))) _Objty(std::forward<_Types>(_Args)...);
		}

		template <class _Uty>
		void destroy(_Uty* const _Ptr) 
		{
			_Ptr->~_Uty();
		}

		_NODISCARD size_t max_size() const noexcept 
		{
			return static_cast<size_t>(-1) / sizeof(_Ty);
		}
	};

	using CEError = size_t;
	using CEHandle = int_fast64_t;
}

#define CE ConventionEngine::

extern "C"
{
	void InitConventionEngine(size_t memory_size);
	void ClearConventionEngine();
	void QuitConventionEngine();

	void GC_ConventionEngine();

	CE CEHandle GetCEHandle(_In_opt_ CE CEObject* ptr);
	_Ret_maybenull_ CE CEObject* GetCEObject(CE CEHandle handle);

	_Ret_notnull_ const char* GetName(CE CEHandle handle);
	void SetName(CE CEHandle handle, _In_ const char* name);
}

#endif // !__FILE_CONVENTION_ENGINE_INTERNAL
