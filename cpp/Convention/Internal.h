#ifndef CONVENTION_KIT_INTERNAL_H
#define CONVENTION_KIT_INTERNAL_H

#include "Convention/Config.h"

extern std::string TypeNameToClassName(const std::string& str) noexcept;

class TypeClass
{
private:
	using type = const type_info&;
public:
	virtual type GetType() const noexcept
	{
		return typeid(*this);
	}
	virtual std::string ToString() const noexcept
	{
		return TypeNameToClassName(this->GetType().name());
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

extern "C"
{
	extern _Ret_maybenull_ void* FindTargetFlagClassPtr(
		_In_reads_bytes_(length) const void* head, size_t length,
		_In_reads_bytes_(flagLength) const void* targetFlag, size_t flagLength,
		size_t flagOffset
	);
	extern _Ret_maybenull_ AnyClass* FindAnyClassPtr(
		_In_reads_bytes_(length) const void* head, size_t length
	);
}

#pragma region Constexpr/Runtime Choose

template<size_t index, typename TFirst, typename... TArgs>
constexpr decltype(auto) ConstexprGetValue(TFirst&& first, TArgs&&... args)
{
	if (index == 0)
		return std::forward<TFirst>(first);
	return ConstexprGet<index - 1>(std::forward<TArgs>(args)...);
}

template<size_t index, typename TFirst, typename... TArgs>
constexpr decltype(auto) ConstexprGetRef(TFirst& first, TArgs&... args)
{
	if (index == 0)
		return first;
	return ConstexprGetRef<index - 1>(args...);
}

template<size_t index, typename TFirst, typename... TArgs>
struct ConstexprGetTypeIndicator
{
	using tag = typename ConstexprGetTypeIndicator<index - 1, TArgs...>::tag;
	constexpr static bool value = false;
};

template<typename TFirst, typename... TArgs>
struct ConstexprGetTypeIndicator<0, TFirst, TArgs...>
{
	using tag = TFirst;
	constexpr static bool value = true;
};

template<size_t index, typename TFirst, typename... TArgs>
using ConstexprGetType = typename ConstexprGetTypeIndicator<index, TFirst, TArgs...>::tag;

#pragma endregion

namespace ConventionKit
{
	inline int VectorialDirection(bool pr)
	{
		return pr ? 1 : -1;
	}

	inline int VectorialDirection(int pr)
	{
		if (pr > 0)
			return 1;
		else if (pr < 0)
			return -1;
		else
			return 0;
	}

	template<typename TPr, typename TVal>
	auto VectorialValue(const TPr& pr, const TVal& value)
	{
		return value * VectorialDirection(pr);
	}

	bool IsArithmeticType(const type_info& type);
	bool IsStringType(const type_info& type);
	bool IsFloatingType(const type_info& type);
	bool IsIntegralType(const type_info& type);
	bool IsUnsignedIntegralType(const type_info& type);
}

template<typename T>
class ICompare
{
public:
	virtual ~ICompare() {}
	virtual int Compare(T left, T right) const noexcept abstract;
};

template<>
class ICompare<void>
{
public:
	template<typename T>
	int Compare(const T& left, const T& right) const noexcept
	{
		if (left < right)
			return -1;
		else if (right < left)
			return 1;
		return 0;
	}
};

#endif // !CONVENTION_KIT_INTERNAL_H
