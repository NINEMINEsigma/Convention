#ifndef CONVENTION_KIT_ANY_CLASS_INSTANCE_H
#define CONVENTION_KIT_ANY_CLASS_INSTANCE_H

#include "Convention/instance/Interface.h"

template<>
class instance<any_class, true> :public instance<any_class, false>
{
private:
	using TMybase = instance<any_class, false>;
	using TPtr = any_class*;
	std::function<void(TPtr)> destructor;
public:
	const type_info& constructorPtrTypen;
	const type_info& constructorInsTypen;

	auto& Release() noexcept
	{
		this->destructor(this->get());
		this->reset();
		return *this;
	}

	// no destructor and ptr is nullptr
	explicit instance(nullptr_t)
		:TMybase(nullptr), destructor([](TPtr) {}),
		constructorPtrTypen(typeid(TPtr)), constructorInsTypen(typeid(any_class)) {}
	// no destructor and ptr must not nullptr
	template<typename TRealType>
	instance(_In_ TRealType* ptr)
		: TMybase(ptr), destructor([](TPtr) {}),
		constructorPtrTypen(typeid(TRealType*)), constructorInsTypen(typeid(*ptr)) {}

	// has destructor and ptr is must not nullptr
	template<typename TRealType>
	instance(_In_ TRealType* ptr, std::function<void(TPtr)> destructor)
		: TMybase(ptr), destructor(destructor),
		constructorPtrTypen(typeid(TRealType*)), constructorInsTypen(typeid(*ptr)) {}
	// has destructor and ptr is must not nullptr
	template<typename TRealType>
	instance(_In_ TRealType* ptr, std::function<void(TRealType*)> destructor)
		: TMybase(ptr), destructor([destructor](TPtr ptr)
			{
				destructor(static_cast<TRealType*>(ptr));
			}), constructorPtrTypen(typeid(TRealType*)), constructorInsTypen(typeid(*ptr)) {}

	// has destructor and ptr is nullptr
	instance(nullptr_t, std::function<void(TPtr)> destructor)
		: instance(nullptr)
	{
		this->destructor = destructor;
	}
	// has destructor and ptr is nullptr
	template<typename TRealType>
	instance(nullptr_t, std::function<void(TRealType*)> destructor)
		: TMybase(nullptr), destructor([destructor](TPtr ptr)
			{
				destructor(reinterpret_cast<TRealType*>(ptr));
			}), constructorPtrTypen(typeid(TRealType*)), constructorInsTypen(typeid(any_class)) {}
public:
	instance(instance&& other) noexcept
		:TMybase(std::move(other)), destructor(other.destructor),
		constructorPtrTypen(other.constructorPtrTypen), constructorInsTypen(other.constructorInsTypen) {}
public:
	virtual ~instance()
	{
		this->destructor(this->get());
	}
};

#endif // !CONVENTION_KIT_ANY_CLASS_INSTANCE_H
