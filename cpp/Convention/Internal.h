#ifndef __FILE_INTERNAL
#define __FILE_INTERNAL

#include "Config.h"

extern std::string typename2classname(const std::string& str) noexcept;

class type_class
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
		return typename2classname(this->GetType().name());
	}
	virtual std::string SymbolName() const noexcept
	{
		return this->GetType().name();
	}
	virtual bool Is(type typen) const noexcept
	{
		return this->GetType() == typen;
	}
	template<typename _T>
	decltype(auto) As() noexcept
	{
		if constexpr (std::is_pointer_v<_T>)
			return dynamic_cast<_T>(this);
		else if constexpr (std::is_reference_v<_T>)
			return dynamic_cast<_T>(*this);
		else
			return static_cast<_T>(*this);
	}

	template<typename _Arg>
	type_class& IfIam(type typen, std::function<void(_Arg)> call)
	{
		if (this->Is(typen))
		{
			if constexpr (std::is_same_v<_Arg, void>)
				call();
			else if constexpr (std::is_same_v<std::decay_t<_Arg>, type_class&>)
				call(*this);
			else if constexpr (std::is_same_v<_Arg, type_class*>)
				call(this);
			else
				call(this->As<_Arg>());
		}
		return *this;
	}

	template<typename _T>
	decltype(auto) Fetch(_T&& from) const noexcept
	{
		return std::forward<_T>(from);
	}
	template<typename _T>
	decltype(auto) Share(_Out_ _T& obj)
	{
		obj = this->As<_T>();
		return *this;
	}
protected:

};
//11100010*8
constexpr size_t any_class_head_flag = 0b1110001011100010111000101110001011100010111000101110001011100010;
class any_class : public type_class
{
private:
	size_t __internal_flag = any_class_head_flag;
public:
};

template <class _Elem, class _Traits, class _Ty>
std::basic_ostream<_Elem, _Traits>& operator<<(std::basic_ostream<_Elem, _Traits>& _Out, const type_class& tc)
{
	_Out << tc.ToString();
	return _Out;
}

namespace convention_kit
{
	template<typename _Ret, typename... _Args>
	std::function<_Ret(_Args...)> make_function(_Ret(*func)(_Args...))
	{
		return func;
	}
	template<typename _Ret, typename... _Args, typename _Func>
	std::function<_Ret(_Args...)> make_function(_Func func)
	{
		return func;
	}
}

extern "C"
{
	extern _Ret_maybenull_ void* find_target_flag_class_ptr(
		_In_reads_bytes_(length) const void* head, size_t length,
		_In_reads_bytes_(flag_length) const void* target_flag, size_t flag_length,
		size_t flag_offset
	);
	extern _Ret_maybenull_ any_class* find_any_class_ptr(
		_In_reads_bytes_(length) const void* head, size_t length
	);
}

#pragma region Constepxr/Runtime Choose

template<size_t index,typename _First,typename... Args>
constexpr decltype(auto) constexpr_get_value(_First&& first, Args&&... args)
{
	if (index == 0)
		return std::forward<_First>(first);
	return constexpr_get<index - 1>(std::forward<Args>(args)...);
}
template<size_t index, typename _First, typename... Args>
constexpr decltype(auto) constexpr_get_ref(_First& first, Args&... args)
{
	if (index == 0)
		return first;
	return constexpr_get_ref<index - 1>(args...);
}
template<size_t index, typename _First, typename... Args>
struct constexpr_get_type_indicator
{
	using tag = typename constexpr_get_type_indicator<index - 1, Args...>::tag;
	constexpr static bool value = false;
};
template<typename _First, typename... Args>
struct constexpr_get_type_indicator<0, _First, Args...>
{
	using tag = _First;
	constexpr static bool value = true;
};
template<size_t index, typename _First, typename... Args>
using constexpr_get_type = typename constexpr_get_type_indicator<index, _First, Args...>::tag;

#pragma endregion

namespace convention_kit
{
	inline int vectorial_direction(bool pr)
	{
		return pr ? 1 : -1;
	}
	inline int vectorial_direction(int pr)
	{
		if (pr > 0)
			return 1;
		else if (pr < 0)
			return -1;
		else
			return 0;
	}
	template<typename _Pr,typename _Val>
	auto vectorial_value(const _Pr& pr, const _Val& value)
	{
		return value * vectorial_direction(pr);
	}
}

#endif // !__FILE_INTERNAL
