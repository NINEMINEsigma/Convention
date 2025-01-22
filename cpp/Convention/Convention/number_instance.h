#ifndef __FILE_CONVENTION_NUMBER_INSTANCE
#define __FILE_CONVENTION_NUMBER_INSTANCE

#include "Convention/Interface.h"

template<typename _NumberType>
class instance<_NumberType, true>: public std::enable_if_t<
	internal::is_number_v<_NumberType>, 
	instance<_NumberType, false>
>
{
private:
	using _MyBase = instance<_NumberType, false>;
public:
	constexpr instance() : _MyBase() {}
	constexpr instance(nullptr_t) : _MyBase(nullptr) {}
	explicit instance(_NumberType* ptr) :_MyBase(ptr) {}
	explicit instance(_NumberType value) :_MyBase(new _NumberType(value)) {}
	template<typename... _Args>
	instance(_Args&&... args) :_MyBase(std::forward<_Args>(args)...) {}
	virtual ~instance() {}

	_NumberType get_cvalue() const noexcept
	{
		if (this->is_empty())
			return **this;
		else
			return 0;
	}
	_NumberType& get_value()
	{
		return **this;
	}

#define operator_core(name)\
	template<typename _R>\
	auto operator##name##(_R&& value) const\
	{\
		if constexpr (internal::is_instance_v<_R>)\
			return **this name static_cast<_NumberType>(*std::forward<_R>(value).get());\
		else\
			return **this name static_cast<_NumberType>(std::forward<_R>(value));\
	}\


	operator_core(+);
	operator_core(-);
	operator_core(/);
	operator_core(==);
	operator_core(!=);
	operator_core(=);
	operator_core(>);
	operator_core(<);
	operator_core(>=);
	operator_core(<=);

#undef operator_core
	template<typename _R>
	auto operator*(_R&& value) const
	{
		if constexpr (internal::is_instance_v<_R>)
			return **this * static_cast<_NumberType>(*std::forward<_R>(value).get());
		else
			return **this * static_cast<_NumberType>(std::forward<_R>(value));
	}
	decltype(auto) operator*() const
	{
		return static_cast<const _MyBase*>(this)->operator*();
	}
#define operator_core(name)\
	template<typename _R>\
	auto operator##name##(_R&& value)\
	{\
		if constexpr (internal::is_instance_v<_R>)\
			**this name static_cast<_NumberType>(*std::forward<_R>(value).get());\
		else\
			**this name static_cast<_NumberType>(std::forward<_R>(value));\
		return **this;\
	}\


	operator_core(+=);
	operator_core(-=);
	operator_core(*=);
	operator_core(/=);

#undef operator_core

	virtual std::string ToString() const noexcept override
	{
		return std::to_string(this->get_cvalue());
	}
};

#endif // !__FILE_CONVENTION_NUMBER_INSTANCE
