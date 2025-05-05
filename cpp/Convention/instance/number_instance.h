#ifndef __FILE_CONVENTION_NUMBER_INSTANCE
#define __FILE_CONVENTION_NUMBER_INSTANCE

#include "Convention/instance/Interface.h"

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
	number_structure() noexcept : molecule(0), denominator(1) {}
	//新增一个构造函数，接受浮点数与精度并处理为分数
	template<typename _Float>
	number_structure(_Float value) noexcept
	{
		// 处理特殊情况
		if (value == 0) 
		{
			molecule = 0;
			denominator = 1;
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
		long long mol = static_cast<long long>(abs_value * precision + 0.5); // +0.5用于四舍五入
		denominator = precision;
		
		// 恢复符号
		if (value < 0) 
		{
			mol = -mol;
		}
		
		// 计算最大公约数进行约分
		size_t gcd_val = std::gcd(std::abs(mol), denominator);
		molecule = mol / static_cast<long long>(gcd_val);
		denominator /= gcd_val;
	}
	
	// 保留带精度参数的构造函数以保持兼容性
	template<typename _Float>
	number_structure(_Float value, size_t precision) noexcept
	{
		// 处理特殊情况
		if (value == 0) {
			molecule = 0;
			denominator = 1;
			return;
		}

		// 将浮点数转换为分数
		_Float abs_value = std::fabs(value);
		long long mol = static_cast<long long>(abs_value * precision + 0.5); // +0.5用于四舍五入
		denominator = precision;
		
		// 恢复符号
		if (value < 0) {
			mol = -mol;
		}
		
		// 计算最大公约数进行约分
		size_t gcd_val = std::gcd(std::abs(mol), denominator);
		molecule = mol / static_cast<long long>(gcd_val);
		denominator /= gcd_val;
	}
	
	number_structure(long long molecule, size_t denominator) noexcept: molecule(molecule), denominator(denominator) {}
	number_structure(const number_structure& other) noexcept : molecule(other.molecule), denominator(other.denominator) {}
	number_structure& operator=(const number_structure& other) noexcept
	{
		this->molecule = other.molecule;
		this->denominator = other.denominator;
		return *this;
	}

	template<typename _NumberType, typename _MidType = long double>
	_NumberType value() const
	{
		return static_cast<_NumberType>(static_cast<_MidType>(molecule) / static_cast<_MidType>(denominator));
	}

	template<typename _NumberType>
	operator _NumberType() const
	{
		return this->value<_NumberType, double>();
	}

	// 符号相关的辅助函数
	bool is_negative() const noexcept
	{
		return molecule < 0;
	}

	bool is_positive() const noexcept
	{
		return molecule > 0;
	}

	bool is_zero() const noexcept
	{
		return molecule == 0;
	}

	long long molecule = 0;
	size_t denominator = 1;
};

template<>
class instance<number_structure, true> :instance<number_structure, false>
{
public:
	using _NumberType = number_structure;
	using _Mybase = instance<_NumberType, false>;
	instance() : _Mybase(new _NumberType()) {}
	instance(_NumberType* ptr) :_Mybase(ptr) {}
	instance(_NumberType value) :_Mybase(new _NumberType(value)) {}
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

	// 辅助函数：简化分数
	inline _NumberType simplify(_NumberType num) const 
	{
		if (num.molecule == 0) return _NumberType(0, 1);
		
		size_t g = std::gcd(std::abs(num.molecule), num.denominator);
		return _NumberType(num.molecule / static_cast<long long>(g), num.denominator / g);
	}

	instance operator+(const instance& other) const 
	{
		const auto& a = *this->get();
		const auto& b = *other.get();

		// 计算最小公倍数作为新分母
		size_t lcm = (a.denominator * b.denominator) / std::gcd(a.denominator, b.denominator);
		
		// 通分后相加
		long long new_molecule = (a.molecule * static_cast<long long>(lcm / a.denominator)) + 
		                          (b.molecule * static_cast<long long>(lcm / b.denominator));
		
		// 简化结果
		return instance(simplify(_NumberType(new_molecule, lcm)));
	}

	instance operator-(const instance& other) const {
		const auto& a = *this->get();
		const auto& b = *other.get();
		
		// 计算最小公倍数作为新分母
		size_t lcm = (a.denominator * b.denominator) / std::gcd(a.denominator, b.denominator);
		
		// 通分后相减
		long long new_molecule = (a.molecule * static_cast<long long>(lcm / a.denominator)) - 
		                          (b.molecule * static_cast<long long>(lcm / b.denominator));
		
		// 简化结果
		return instance(simplify(_NumberType(new_molecule, lcm)));
	}

	instance operator*(const instance& other) const {
		const auto& a = *this->get();
		const auto& b = *other.get();
		
		// 分子分母相乘
		long long new_molecule = a.molecule * b.molecule;
		size_t new_denominator = a.denominator * b.denominator;
		
		// 简化结果
		return instance(simplify(_NumberType(new_molecule, new_denominator)));
	}

	instance operator/(const instance& other) const {
		const auto& a = *this->get();
		const auto& b = *other.get();
		
		if (b.molecule == 0) {
			throw std::runtime_error("除数不能为零");
		}
		
		// 乘以倒数，注意处理分母的符号
		long long new_molecule = a.molecule * static_cast<long long>(b.denominator);
		long long b_mol = b.molecule;
		size_t new_denominator = a.denominator * std::abs(b_mol);
		
		// 处理符号位
		if (b_mol < 0) {
			new_molecule = -new_molecule;
		}
		
		// 简化结果
		return instance(simplify(_NumberType(new_molecule, new_denominator)));
	}

	// 复合赋值运算符
	instance& operator+=(const instance& other) {
		*this = *this + other;
		return *this;
	}

	instance& operator-=(const instance& other) {
		*this = *this - other;
		return *this;
	}

	instance& operator*=(const instance& other) {
		*this = *this * other;
		return *this;
	}

	instance& operator/=(const instance& other) {
		*this = *this / other;
		return *this;
	}

	// 比较运算符
	bool operator==(const instance& other) const {
		// 通分后比较分子是否相等
		const auto& a = *this->get();
		const auto& b = *other.get();
		return (a.molecule * static_cast<long long>(b.denominator)) == 
		       (b.molecule * static_cast<long long>(a.denominator));
	}

	bool operator!=(const instance& other) const {
		return !(*this == other);
	}

	bool operator<(const instance& other) const {
		const auto& a = *this->get();
		const auto& b = *other.get();
		return (a.molecule * static_cast<long long>(b.denominator)) < 
		       (b.molecule * static_cast<long long>(a.denominator));
	}

	bool operator>(const instance& other) const {
		const auto& a = *this->get();
		const auto& b = *other.get();
		return (a.molecule * static_cast<long long>(b.denominator)) > 
		       (b.molecule * static_cast<long long>(a.denominator));
	}

	bool operator<=(const instance& other) const {
		return !(*this > other);
	}

	bool operator>=(const instance& other) const {
		return !(*this < other);
	}

	// 一元运算符
	instance operator-() const {
		const auto& a = *this->get();
		return instance(_NumberType(-a.molecule, a.denominator));
	}

	// 字符串表示
	virtual std::string ToString() const noexcept override {
		const auto& a = *this->get();
		return std::to_string(a.molecule) + "/" + std::to_string(a.denominator);
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
