#ifndef __FILE_CONVENTION_NUMBER_INSTANCE
#define __FILE_CONVENTION_NUMBER_INSTANCE

#include "Convention/Interface.h"

template<typename _Number>
struct number_ex_indicator
{
	static constexpr bool value = 
		//!internal::is_stream_v<_Number> &&
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

#ifdef __REF_BOOST

#include <boost/math_fwd.hpp>
#include <boost/math/quaternion.hpp>
#include <boost/multiprecision/number.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions.hpp>
#include <boost/numeric/interval.hpp>
#include <boost/math/quadrature/gauss.hpp>

template<typename _NumberType>
class extended_number : public instance<number_ex_indicator<_NumberType>, true>
{
private:
	using _Mybase = instance<number_ex_indicator<_NumberType>, true>;
public:
	using _Mybase::_Mybase;  // 继承构造函数

	// 高精度数值类型
	using high_precision_float = boost::multiprecision::cpp_dec_float_100;
	using big_integer = boost::multiprecision::cpp_int;
	using rational = boost::rational<_NumberType>;

	// 特殊数学常量
	static constexpr auto pi() { return boost::math::constants::pi<_NumberType>(); }
	static constexpr auto e() { return boost::math::constants::e<_NumberType>(); }
	static constexpr auto phi() { return boost::math::constants::phi<_NumberType>(); }
	static constexpr auto ln_two() { return boost::math::constants::ln_two<_NumberType>(); }

	// 特殊数学函数
	_NumberType factorial() const
	{
		return boost::math::factorial<_NumberType>(this->get_cvalue());
	}

	_NumberType gamma() const
	{
		return boost::math::tgamma(this->get_cvalue());
	}

	_NumberType bessel_j(int n) const
	{
		return boost::math::cyl_bessel_j(n, this->get_cvalue());
	}

	_NumberType bessel_y(int n) const
	{
		return boost::math::cyl_neumann(n, this->get_cvalue());
	}

	// 数值区间操作
	using interval = boost::numeric::interval<_NumberType>;
	interval make_interval(_NumberType upper) const
	{
		return interval(this->get_cvalue(), upper);
	}

	// 高精度转换
	high_precision_float to_high_precision() const
	{
		return high_precision_float(this->get_cvalue());
	}

	// 有理数操作
	rational to_rational(const _NumberType& denominator) const
	{
		return rational(this->get_cvalue(), denominator);
	}

	// 数值积分
	template<typename F>
	_NumberType integrate(const _NumberType& upper, F&& f) const
	{
		return boost::math::quadrature::gauss<_NumberType>::integrate(
			std::forward<F>(f),
			this->get_cvalue(),
			upper
		);
	}

	// 数值微分
	template<typename F>
	_NumberType derivative(F&& f) const
	{
		const _NumberType h = std::numeric_limits<_NumberType>::epsilon();
		const _NumberType x = this->get_cvalue();
		return (f(x + h) - f(x - h)) / (2 * h);
	}
};

// 常用类型别名
using extended_double = extended_number<double>;
using extended_float = extended_number<float>;
using extended_int = extended_number<int>;
using extended_long = extended_number<long>;

#endif // __REF_BOOST

#endif // !__FILE_CONVENTION_NUMBER_INSTANCE
