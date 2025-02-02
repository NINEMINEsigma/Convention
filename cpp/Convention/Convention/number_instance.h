#ifndef __FILE_CONVENTION_NUMBER_INSTANCE
#define __FILE_CONVENTION_NUMBER_INSTANCE

#include "Convention/Interface.h"

template<typename _Number>
struct number_ex_indicator
{
	static constexpr bool value = 
		!internal::is_stream_v<_Number> &&
		internal::is_number_v<_Number>;
	using tag = std::enable_if_t<value, _Number>;
};
template<typename _NumberType>
class instance<number_ex_indicator<_NumberType>, true> : public instance<_NumberType, false>
{
private:
	using _Mybase = instance<_NumberType, false>;
public:
	constexpr instance() : _Mybase() {}
	constexpr instance(nullptr_t) : _Mybase(nullptr) {}
	explicit instance(_NumberType* ptr) :_Mybase(ptr) {}
	explicit instance(_NumberType value) :_Mybase(new _NumberType(value)) {}
	explicit instance(typename _Mybase::_shared& rv) :_Mybase(rv) {}
	explicit instance(typename _Mybase::_shared&& rv) :_Mybase(std::move(rv)) {}
	template<typename... _Args>
	instance(_Args&&... args) : _Mybase(std::forward<_Args>(args)...) {}
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
	auto operator name (_R&& value) const\
	{\
		if constexpr (internal::is_instance_v<_R>)\
			return **this name static_cast<_NumberType>(*std::forward<_R>(value).get());\
		else\
			return **this name static_cast<_NumberType>(std::forward<_R>(value));\
	}\


	operator_core(+);
	operator_core(-);
	operator_core(/ );
	operator_core(== );
	operator_core(!= );
	operator_core(= );
	operator_core(> );
	operator_core(< );
	operator_core(>= );
	operator_core(<= );

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
		return static_cast<const _Mybase*>(this)->operator*();
	}
#define operator_core(name)\
	template<typename _R>\
	auto operator name (_R&& value)\
	{\
		if constexpr (internal::is_instance_v<_R>)\
			**this name static_cast<_NumberType>(*std::forward<_R>(value).get());\
		else\
			**this name static_cast<_NumberType>(std::forward<_R>(value));\
		return **this;\
	}\


	operator_core(+= );
	operator_core(-= );
	operator_core(*= );
	operator_core(/= );

#undef operator_core

	virtual std::string ToString() const noexcept override
	{
		return std::to_string(this->get_cvalue());
	}
};

namespace internal
{
	template<typename _Type>
	class range:public any_class
	{
	public:
		struct range_iteratir
		{
			range_iteratir(_Type& val, _Type& step) :__init(val), __init(step) {}
			_Type& val, step;
			auto& operator++(void)
			{
				val += step;
				return *this;
			}
			bool operator!=(const range_iteratir& iter)
			{
				return this->val != iter.val;
			}
			_Type& operator*(void)
			{
				return val;
			}
		};

		_Type head, tail, step;
		range(_Type head, _Type tail, _Type step) :__init(head), __init(tail), __init(step) {}
		range(_Type boundary) :range(0, boundary, 1) {}
		range(_Type boundary, _Type step) :range(0, boundary, step) {}
		auto begin()
		{
			return range_iteratir(head, step);
		}
		auto end()
		{
			return range_iteratir(tail, step);
		}
	};
}

#endif // !__FILE_CONVENTION_NUMBER_INSTANCE
