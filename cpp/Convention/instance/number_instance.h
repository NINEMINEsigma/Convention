﻿#ifndef __FILE_CONVENTION_NUMBER_INSTANCE
#define __FILE_CONVENTION_NUMBER_INSTANCE

#include "Convention/instance/Interface.h"
#include "Convention/instance/eval/eval_init.hpp"

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

	_NumberType gcd(_NumberType right) const noexcept
	{
		return std::gcd(this->get_cvalue(), right.get_cvalue());
	}
};

struct number_structure
{
	number_structure() noexcept : molecule(0), denominator(1), sign(false) {}
	//新增一个构造函数，接受浮点数与精度并处理为分数
	template<typename _Float>
	number_structure(_Float value) noexcept
	{
		// 处理特殊情况
		if (value == 0) 
		{
			molecule = 0;
			denominator = 1;
			sign = false;
			return;
		}

		// 获取浮点数的小数部分
		_Float integral_part;
		_Float fractional_part = std::fabs(std::modf(std::fabs(value), &integral_part));

		// 确定精度：基于小数位数和数值范围自动计算
		size_t precision = 1;
		if (fractional_part > 0) 
		{
			// 获取浮点类型的有效数字位数
			const int digits = std::numeric_limits<_Float>::digits10;
			
			// 计算小数点后的位数
			_Float temp = fractional_part;
			int decimal_places = 0;
			
			// 查找最后一个非零小数位
			while (temp > 0 && decimal_places < digits) 
			{
				temp = temp * 10;
				int digit = static_cast<int>(temp);
				temp -= digit;
				decimal_places++;
				if (digit != 0) 
				{
					// 当我们找到最后一个非零数字时，设置适当的精度
					precision = static_cast<size_t>(std::pow(10, decimal_places));
				}
			}
			
			// 最低精度保证为1000，防止太小的精度导致精度损失
			precision = std::max(precision, static_cast<size_t>(1000));
		}
		
		// 将浮点数转换为分数
		_Float abs_value = std::fabs(value);
		molecule = static_cast<size_t>(abs_value * precision + 0.5); // +0.5用于四舍五入
		denominator = precision;
		sign = (value < 0);
		
		// 计算最大公约数进行约分
		size_t gcd_val = std::gcd(molecule, denominator);
		molecule /= gcd_val;
		denominator /= gcd_val;
	}
	
	// 保留带精度参数的构造函数以保持兼容性
	template<typename _Float>
	number_structure(_Float value, size_t precision) noexcept
	{
		// 处理特殊情况
		if (value == 0) 
		{
			molecule = 0;
			denominator = 1;
			sign = false;
			return;
		}

		// 将浮点数转换为分数
		_Float abs_value = std::fabs(value);
		molecule = static_cast<size_t>(abs_value * precision + 0.5); // +0.5用于四舍五入
		denominator = precision;
		sign = (value < 0);
		
		// 计算最大公约数进行约分
		size_t gcd_val = std::gcd(molecule, denominator);
		molecule /= gcd_val;
		denominator /= gcd_val;
	}
	
	number_structure(size_t mol, size_t den, bool is_negative = false) noexcept
		: molecule(mol), denominator(den), sign(is_negative) {}
	number_structure(const number_structure& other) noexcept 
		: molecule(other.molecule), denominator(other.denominator), sign(other.sign) {}
	number_structure& operator=(const number_structure& other) noexcept
	{
		this->molecule = other.molecule;
		this->denominator = other.denominator;
		this->sign = other.sign;
		return *this;
	}

	template<typename _NumberType, typename _MidType = long double>
	_NumberType value() const
	{
		_MidType result = static_cast<_MidType>(molecule) / static_cast<_MidType>(denominator);
		return static_cast<_NumberType>(sign ? -result : result);
	}

	template<typename _NumberType>
	operator _NumberType() const
	{
		return this->value<_NumberType, double>();
	}

	// 符号相关的辅助函数
	bool is_negative() const noexcept
	{
		return sign && molecule != 0;
	}

	bool is_positive() const noexcept
	{
		return !sign && molecule != 0;
	}

	bool is_zero() const noexcept
	{
		return molecule == 0;
	}

	// 辅助函数：简化分数
	inline number_structure simplify() const noexcept
	{
		if (molecule == 0) return number_structure(0, 1, false);
		
		size_t g = std::gcd(molecule, denominator);
		return number_structure(molecule / g, denominator / g, sign);
	}

	inline number_structure& let_simplify() noexcept
	{
		if (molecule == 0) return *this;

		size_t g = std::gcd(molecule, denominator);
		molecule = molecule / g;
		denominator = denominator / g;
		return *this;
	}

	// 加法运算符
	number_structure operator+(const number_structure& other) const noexcept
	{
		// 计算最小公倍数作为新分母
		size_t lcm = (denominator * other.denominator) / std::gcd(denominator, other.denominator);

		// 通分后的分子值
		size_t a_mol = molecule * (lcm / denominator);
		size_t b_mol = other.molecule * (lcm / other.denominator);

		// 考虑符号进行加法运算
		if (sign == other.sign)
		{
			return number_structure(a_mol + b_mol, lcm, sign).simplify();
		}
		else
		{
			bool stats = a_mol >= b_mol;
			return number_structure(stats ? (a_mol - b_mol) : (b_mol - a_mol), lcm, stats ? sign : other.sign).simplify();
		}
	}

	// 减法运算符
	number_structure operator-(const number_structure& other) const noexcept
	{
		// 计算最小公倍数作为新分母
		size_t lcm = (denominator * other.denominator) / std::gcd(denominator, other.denominator);
		
		// 通分后的分子值
		size_t a_mol = molecule * (lcm / denominator);
		size_t b_mol = other.molecule * (lcm / other.denominator);
		
		// 考虑符号进行减法运算
		if (sign != other.sign) 
		{
			return number_structure(a_mol + b_mol, lcm, sign).simplify();
		}
		else 
		{
			bool stats = a_mol >= b_mol;
			return number_structure(stats ? (a_mol - b_mol) : (b_mol - a_mol), lcm, stats ? sign : !sign).simplify();
		}
	}

	// 乘法运算符
	number_structure operator*(const number_structure& other) const noexcept
	{	
		return number_structure(molecule * other.molecule, denominator * other.denominator, sign == other.sign).simplify();
	}

	template<typename _NumberTy>
	number_structure operator*(const _NumberTy& other) const noexcept
	{
		if constexpr (std::is_integral_v<_NumberTy>)
		{
			return number_structure(molecule * other, denominator, sign == (other > 0)).simplify();
		}
		else
		{
			return *this * number_structure(other);
		}
	}

	// 除法运算符
	number_structure operator/(const number_structure& other) const
	{
		if (other.molecule == 0)
		{
			throw std::runtime_error("The divisor cannot be zero");
		}

		return number_structure(molecule * other.denominator, denominator * other.molecule, sign == other.sign).simplify();
	}

	template<typename _NumberTy>
	number_structure operator/(const _NumberTy& other) const
	{
		if (other == 0)
		{
			throw std::runtime_error("The divisor cannot be zero");
		}

		return *this * (1.0 / other);
	}

	// 复合赋值运算符
	number_structure& operator+=(const number_structure& other) noexcept
	{
		*this = *this + other;
		return *this;
	}

	number_structure& operator-=(const number_structure& other) noexcept
	{
		*this = *this - other;
		return *this;
	}

	template<typename _NumberTy>
	number_structure& operator*=(const _NumberTy& other) noexcept
	{
		*this = *this * other;
		return *this;
	}

	template<typename _NumberTy>
	number_structure& operator/=(const _NumberTy& other)
	{
		*this = *this / other;
		return *this;
	}

	int compare(const number_structure& other) const noexcept
	{
		if (sign != other.sign) 
		{
			return sign ? -1.0 : 1.0;
		}

		size_t a_compare = molecule * other.denominator;
		size_t b_compare = other.molecule * denominator;

		if (a_compare == b_compare)
			return 0.0;

		return (a_compare > b_compare) ? 1.0 : -1.0; 
	}

	template<typename _Ret = double>
	_Ret coefficient(const number_structure& data) const noexcept
	{
		return (data / *this).value<_Ret>();
	}

	// 比较运算符
	bool operator==(const number_structure& other) const noexcept
	{
		// 考虑符号和值进行比较
		if (molecule == 0 && other.molecule == 0) 
			return true;
		
		return molecule * other.denominator == other.molecule * denominator;
	}

	bool operator!=(const number_structure& other) const noexcept
	{
		return !(*this == other);
	}

	bool operator<(const number_structure& other) const noexcept
	{
		return compare(other) < 0;
	}

	bool operator>(const number_structure& other) const noexcept
	{
		return compare(other) > 0;
	}

	bool operator<=(const number_structure& other) const noexcept
	{
		return !(*this > other);
	}

	bool operator>=(const number_structure& other) const noexcept
	{
		return !(*this < other);
	}

	static number_structure parse(const std::string& str);

	// 一元运算符
	number_structure operator-() const noexcept
	{
		return number_structure(molecule, denominator, !sign);
	}

	// 字符串表示
	std::string ToString() const noexcept
	{
		return (sign ? "-" : "") + std::to_string(molecule) + "/" + std::to_string(denominator);
	}

	size_t molecule = 0;
	size_t denominator = 1;
	bool sign = false; // false表示正数，true表示负数
};

number_structure number_structure::parse(const std::string& str)
{
	// 创建double类型的计算器实例
	auto calc = eval_init::create_real_eval<double>();

	// 解析表达式
	auto expr = calc.parse(str);

	// 计算表达式
	return calc.evaluate(expr);
}

auto operator "" _eval(const char* str)
{
	return number_structure::parse(str);
}

template<>
class instance<number_structure, true> :instance<number_structure, false>
{
public:
	using _NumberType = number_structure;
	using _Mybase = instance<_NumberType, false>;
	instance() : _Mybase(new _NumberType()) {}
	instance(_NumberType* ptr) :_Mybase(ptr) {}
	instance(_NumberType value) :_Mybase(new _NumberType(value)) {}
	instance(const std::string& str) : _Mybase(new _NumberType(_NumberType::parse(str))) {}
	virtual ~instance() {}

	template<typename _Ret, typename _MidType = long double>
	_Ret value() const
	{
		return this->get()->value<_Ret, _MidType>();
	}

	template<typename _Ret>
	operator _Ret() const
	{
		return this->value<_Ret, double>();
	}

	// 通过使用number_structure内部的操作符进行转发
	instance operator+(const instance& other) const 
	{
		return instance(*this->get() + *other.get());
	}

	instance operator-(const instance& other) const 
	{
		return instance(*this->get() - *other.get());
	}

	instance operator*(const instance& other) const 
	{
		return instance(*this->get() * *other.get());
	}

	instance operator/(const instance& other) const 
	{
		return instance(*this->get() / *other.get());
	}

	// 复合赋值运算符
	instance& operator+=(const instance& other) 
	{
		**this += *other.get();
		return *this;
	}

	instance& operator-=(const instance& other) 
	{
		**this -= *other.get();
		return *this;
	}

	instance& operator*=(const instance& other) 
	{
		**this *= *other.get();
		return *this;
	}

	instance& operator/=(const instance& other) 
	{
		**this /= *other.get();
		return *this;
	}

	// 比较运算符
	bool operator==(const instance& other) const 
	{
		return **this == *other.get();
	}

	bool operator!=(const instance& other) const 
	{
		return **this != *other.get();
	}

	bool operator<(const instance& other) const 
	{
		return **this < *other.get();
	}

	bool operator>(const instance& other) const 
	{
		return **this > *other.get();
	}

	bool operator<=(const instance& other) const 
	{
		return **this <= *other.get();
	}

	bool operator>=(const instance& other) const 
	{
		return **this >= *other.get();
	}

	// 一元运算符
	instance operator-() const 
	{
		return instance(-(**this));
	}

	// 字符串表示
	virtual std::string ToString() const noexcept override 
	{
		return this->get()->ToString();
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
#if __REF_BOOST
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
class number_traits
{
public:
	// 高精度数值类型
	using high_precision_float = boost::multiprecision::cpp_dec_float_100;
	using big_integer = boost::multiprecision::cpp_int;
	using rational = boost::rational<_NumberType>;

	// 特殊数学常量
	static constexpr auto pi() { return boost::math::constants::pi<_NumberType>(); }
	static constexpr auto e() { return boost::math::constants::e<_NumberType>(); }
	static constexpr auto phi() { return boost::math::constants::phi<_NumberType>(); }
	static constexpr auto ln_two() { return boost::math::constants::ln_two<_NumberType>(); }

	// 阶乘
	static _NumberType factorial(_NumberType value)
	{
		return boost::math::factorial<_NumberType>(value);
	}

	// 伽马函数
	static _NumberType gamma(_NumberType value)
	{
		return boost::math::tgamma(value);
	}

	// 贝塞尔函数 J
	static _NumberType bessel_j(int n, _NumberType value)
	{
		return boost::math::cyl_bessel_j(n, value);
	}

	// 贝塞尔函数 Y
	static _NumberType bessel_y(int n, _NumberType value)
	{
		return boost::math::cyl_neumann(n, value);
	}

	// 数值区间操作
	using interval = boost::numeric::interval<_NumberType>;
	static interval make_interval(_NumberType lower, _NumberType upper)
	{
		return interval(lower, upper);
	}

	// 高精度转换
	static high_precision_float to_high_precision(_NumberType value)
	{
		return high_precision_float(value);
	}

	// 有理数操作
	static rational to_rational(_NumberType numerator, _NumberType denominator)
	{
		return rational(numerator, denominator);
	}
	static _NumberType from_rational(rational& r)
	{
		return static_cast<_NumberType>(r);	
	}

	// 数值积分
	template<typename F>
	static _NumberType integrate(const _NumberType& upper, F&& f, _NumberType lower = 0)
	{
		return boost::math::quadrature::gauss<_NumberType>::integrate(
			std::forward<F>(f),
			lower,
			upper
		);
	}

	// 数值微分
	template<typename F>
	static _NumberType derivative(F&& f, _NumberType x)
	{
		const _NumberType h = std::numeric_limits<_NumberType>::epsilon();
		return (f(x + h) - f(x - h)) / (2 * h);
	}
};

// 常用类型别名
using double_traits = number_traits<double>;
using float_traits = number_traits<float>;
using int_traits = number_traits<int>;
using long_traits = number_traits<long>;
using long_long_traits = number_traits<long long>;
using unsigned_int_traits = number_traits<unsigned int>;
using unsigned_long_traits = number_traits<unsigned long>;
using unsigned_long_long_traits = number_traits<unsigned long long>;
using short_traits = number_traits<short>;
using unsigned_short_traits = number_traits<unsigned short>;
using char_traits = number_traits<char>;
using unsigned_char_traits = number_traits<unsigned char>;
using long_double_traits = number_traits<long double>;
using size_t_traits = number_traits<size_t>;

#endif //__REF_BOOST

#endif // __REF_BOOST

#endif // !__FILE_CONVENTION_NUMBER_INSTANCE
