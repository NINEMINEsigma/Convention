#ifndef __FILE_CONVENTION_VOID_INSTANCE
#define __FILE_CONVENTION_VOID_INSTANCE

#include "Convention/Interface.h"

template<>
class instance<any_class, true> :public instance<any_class, false>
{
	using _Ptr = any_class*;
	std::function<void(_Ptr)> destructor;
public:
	const type_info& constructor_ptr_typen;
	const type_info& constructor_ins_typen;
	using _MyBase = instance<any_class, false>;

	auto& release() noexcept
	{
		this->destructor(this->get());
		this->reset();
		return *this;
	}

	// no destructor and ptr is nullptr
	explicit instance(nullptr_t)
		:_MyBase(nullptr), destructor([](_Ptr) {}),
		constructor_ptr_typen(typeid(_Ptr)), constructor_ins_typen(typeid(any_class)) {}
	// no destructor and ptr must not nullptr
	template<typename _RealType>
	instance(_In_ _RealType* ptr)
		: _MyBase(ptr), destructor([](_Ptr) {}),
		constructor_ptr_typen(typeid(_RealType*)), constructor_ins_typen(typeid(*ptr)) {}

	// has destructor and ptr is must not nullptr
	template<typename _RealType>
	instance(_In_ _RealType* ptr, std::function<void(_Ptr)> destructor)
		: _MyBase(ptr), __init(destructor),
		constructor_ptr_typen(typeid(_RealType*)), constructor_ins_typen(typeid(*ptr)) {}
	// has destructor and ptr is must not nullptr
	template<typename _RealType>
	instance(_In_ _RealType* ptr, std::function<void(_RealType*)> destructor)
		: _MyBase(ptr), destructor([destructor](_Ptr ptr)
			{
				destructor(static_cast<_RealType*>(ptr));
			}), constructor_ptr_typen(typeid(_RealType*)), constructor_ins_typen(typeid(*ptr)) {}

	// has destructor and ptr is nullptr
	instance(nullptr_t, std::function<void(_Ptr)> destructor)
		: instance(nullptr) 
	{
		this->destructor = destructor;
	}
	// has destructor and ptr is nullptr
	template<typename _RealType>
	instance(nullptr_t, std::function<void(_RealType*)> destructor)
		: _MyBase(nullptr), destructor([destructor](_Ptr ptr)
			{
				destructor(reinterpret_cast<_RealType*>(ptr));
			}), constructor_ptr_typen(typeid(_RealType*)), constructor_ins_typen(typeid(any_class)) {}
public:
	instance(instance&& other) noexcept
		:_MyBase(std::move(other)), destructor(other.destructor),
		constructor_ptr_typen(other.constructor_ptr_typen), constructor_ins_typen(other.constructor_ins_typen) {}
public:
	virtual ~instance()
	{
		this->destructor(this->get());
	}
};

#endif // !__FILE_CONVENTION_VOID_INSTANCE
