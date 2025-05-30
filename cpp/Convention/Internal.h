#ifndef CONVENTION_KIT_INTERNAL_H
#define CONVENTION_KIT_INTERNAL_H

#include "Convention/Config.h"


enum class TypeFlag
{
	Unknown = 0,
	Arithmetic = 1 << 0,
	Floating = 1 << 1,
	Integral = 1 << 2,
	Unsigned = 1 << 3,
	String = 1<<4
};


class TypeClass
{
private:
	using type = const type_info&;
public:
	static std::string Typeame2Classname(const std::string& str) noexcept
	{
		if (str.substr(0, 6) == "class ")
			return str.substr(6);
		else if (str.substr(0, 7) == "struct ")
			return str.substr(7);
		return str;
	}
	
	static TypeFlag GetTypeFlag(const type_info& type)
	{
		if (
			type == typeid(int) ||
			type == typeid(unsigned int) ||
			type == typeid(long) ||
			type == typeid(unsigned long) ||
			type == typeid(long long) ||
			type == typeid(unsigned long long) ||
			type == typeid(float) ||
			type == typeid(double) ||
			type == typeid(long double))
		{
			TypeFlag result = TypeFlag::Arithmetic;

		}
		if (
			type == typeid(float) ||
			type == typeid(double) ||
			type == typeid(long double))
		{
			return static_cast<TypeFlag>(
				(size_t)TypeFlag::Arithmetic | (size_t)TypeFlag::Floating
				);
		}
		if (
			type == typeid(short) ||
			type == typeid(int) ||
			type == typeid(long) ||
			type == typeid(long long))
		{
			return static_cast<TypeFlag>(
				(size_t)TypeFlag::Arithmetic | (size_t)TypeFlag::Integral
				);
		}
		if (
			type == typeid(unsigned short) ||
			type == typeid(unsigned int) ||
			type == typeid(unsigned long) ||
			type == typeid(unsigned long long))
		{
			return static_cast<TypeFlag>(
				(size_t)TypeFlag::Arithmetic | (size_t)TypeFlag::Integral |
				(size_t)TypeFlag::Unknown
				);
		}
		if (
			type == typeid(std::string) ||
			type == typeid(std::wstring) ||
			type == typeid(std::u16string) ||
			type == typeid(std::u32string) ||
			type == typeid(std::string_view) ||
			type == typeid(std::wstring_view) ||
			type == typeid(std::u16string_view) ||
			type == typeid(std::u32string_view) ||
			type == typeid(const char*)
			)
		{
			return TypeFlag::String;
		}
		return TypeFlag::Unknown;
	}
	template<typename T>
	static constexpr TypeFlag GetTypeFlag()
	{
		if constexpr(
			std::is_same_v<float, T> ||
			std::is_same_v<double, T> ||
			std::is_same_v<long double, T>)
		{
			return static_cast<TypeFlag>(
				(size_t)TypeFlag::Arithmetic | (size_t)TypeFlag::Floating
				);
		}
		if (
			std::is_same_v<short, T> ||
			std::is_same_v<int, T> ||
			std::is_same_v<long, T> ||
			std::is_same_v<long long, T>)
		{
			return static_cast<TypeFlag>(
				(size_t)TypeFlag::Arithmetic | (size_t)TypeFlag::Integral
				);
		}
		if (
			std::is_same_v<unsigned short, T> ||
			std::is_same_v<unsigned int, T> ||
			std::is_same_v<unsigned long, T> ||
			std::is_same_v<unsigned long long, T>)
		{
			return static_cast<TypeFlag>(
				(size_t)TypeFlag::Arithmetic | (size_t)TypeFlag::Integral |
				(size_t)TypeFlag::Unknown
				);
		}
		if (
			std::is_same_v<std::string, T> ||
			std::is_same_v<std::wstring, T> ||
			std::is_same_v<std::u16string, T> ||
			std::is_same_v<std::u32string, T> ||
			std::is_same_v<std::string_view, T> ||
			std::is_same_v<std::wstring_view, T> ||
			std::is_same_v<std::u16string_view, T> ||
			std::is_same_v<std::u32string_view, T> ||
			std::is_same_v<const char*, T>)
		{
			return TypeFlag::String;
		}
		return TypeFlag::Unknown;
	}

	virtual type GetType() const noexcept
	{
		return typeid(*this);
	}
	virtual std::string ToString() const noexcept
	{
		return Typeame2Classname(this->GetType().name());
	}
	virtual std::string SymbolName() const noexcept
	{
		return this->GetType().name();
	}
	virtual bool Is(type typen) const noexcept
	{
		return this->GetType() == typen;
	}
	template<typename T>
	decltype(auto) As() noexcept
	{
		if constexpr (std::is_pointer_v<T>)
			return dynamic_cast<T>(this);
		else if constexpr (std::is_reference_v<T>)
			return dynamic_cast<T>(*this);
		else
			return static_cast<T>(*this);
	}

	template<typename TArg>
	TypeClass& IfIam(type typen, std::function<void(TArg)> call)
	{
		if (this->Is(typen))
		{
			if constexpr (std::is_same_v<TArg, void>)
				call();
			else if constexpr (std::is_same_v<std::decay_t<TArg>, TypeClass&>)
				call(*this);
			else if constexpr (std::is_same_v<TArg, TypeClass*>)
				call(this);
			else
				call(this->As<TArg>());
		}
		return *this;
	}

	template<typename T>
	decltype(auto) Fetch(T&& from) const noexcept
	{
		return std::forward<T>(from);
	}
	template<typename T>
	decltype(auto) Share(_Out_ T& obj)
	{
		obj = this->As<T>();
		return *this;
	}
protected:

};
//11100010*8
constexpr size_t ANY_CLASS_HEAD_FLAG = 0b1110001011100010111000101110001011100010111000101110001011100010;
class AnyClass : public TypeClass
{
private:
	size_t internalFlag = ANY_CLASS_HEAD_FLAG;
public:
	static _Ret_maybenull_ void* FindTargetFlagClassPtr(
		_In_reads_bytes_(length) const void* head, size_t length,
		_In_reads_bytes_(flagLength) const void* targetFlag, size_t flagLength,
		size_t flagOffset
	)
	{
		for (size_t i = 0; i != length; i++)
		{
			if (0 == memcmp(reinterpret_cast<void*>(reinterpret_cast<size_t>(head) + i), targetFlag, flagLength))
			{
				return reinterpret_cast<void*>(reinterpret_cast<size_t>(head) + i - flagOffset);
			}
		}
		return nullptr;
	}
	static _Ret_maybenull_ AnyClass* FindAnyClassPtr(
		_In_reads_bytes_(length) const void* head, size_t length
	)
	{
		//any_class's flag is not at 0-offset, so we begin in 1 index
		for (size_t i = 1; i != length; i++)
		{
			if (*reinterpret_cast<size_t*>(reinterpret_cast<size_t>(head) + i) == ANY_CLASS_HEAD_FLAG)
			{
				return reinterpret_cast<AnyClass*>(reinterpret_cast<size_t>(head) + i - sizeof(size_t));
			}
		}
		return nullptr;
	}
};

template <class TElem, class TTraits, class TType>
std::basic_ostream<TElem, TTraits>& operator<<(std::basic_ostream<TElem, TTraits>& out, const TypeClass& tc)
{
	out << tc.ToString();
	return out;
}

namespace ConventionKit
{
	template<typename TRet, typename... TArgs>
	std::function<TRet(TArgs...)> MakeFunction(TRet(*func)(TArgs...))
	{
		return func;
	}
	template<typename TRet, typename... TArgs, typename TFunc>
	std::function<TRet(TArgs...)> MakeFunction(TFunc func)
	{
		return func;
	}
}

namespace ConventionKit
{
	template<typename T, typename _ICompare = ICompare<void>>
	inline int VectorialDirection(T&& pr, const _ICompare& ic = _ICompare())
	{
		return ic.Compare(pr, 0);
	}

	template<typename TPr, typename TVal>
	auto VectorialValue(const TPr& pr, const TVal& value)
	{
		return value * VectorialDirection(pr);
	}
}

#endif // !CONVENTION_KIT_INTERNAL_H
