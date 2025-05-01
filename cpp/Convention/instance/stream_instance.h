#ifndef __FILE_CONVENTION_IOSTREAM_INSTANCE
#define __FILE_CONVENTION_IOSTREAM_INSTANCE

#include "Convention/instance/Interface.h"

template<>
class instance<std::ios_base, true> :public instance<std::ios_base, false>
{
	using _Mybase = instance< std::ios_base, false>;
public:
	explicit instance(_shared& rv) :_Mybase(rv) {}
	explicit instance(_shared&& rv) :_Mybase(std::move(rv)) {}
	template<typename... _Args>
	instance(_Args&&... args) :_Mybase(std::forward<_Args>(args)...) {}
	instance_move_operator(public)
	{

	}
	virtual ~instance()
	{
		auto ofptr = dynamic_cast<std::ofstream*>(this->get());
		if (ofptr != nullptr)
		{
			ofptr->close();
			return;
		}
		auto ifptr = dynamic_cast<std::ifstream*>(this->get());
		if (ifptr != nullptr)
		{
			ifptr->close();
			return;
		}
	}

	template<typename _Right>
	decltype(auto) operator<<(_Right&& value)
	{
		dynamic_cast<std::ostream&>(**this) << value;
		return **this;
	}
	template<typename _Right>
	decltype(auto) operator>>(_Right& value)
	{
		dynamic_cast<std::istream&>(**this) >> value;
		return **this;
	}
};
template<template<typename,typename> class _ST, typename _Elem>
class instance<_ST<_Elem, std::char_traits<_Elem>>, true> :public instance< _ST<_Elem, std::char_traits<_Elem>>, false>
{
public:
	using _Stream = _ST<_Elem, std::char_traits<_Elem>>;

	using _Mybase = instance< _Stream, false>;
	template<typename _Str>
	instance(_Str&& path, std::ios::openmode mode) : _Mybase(new _Stream(std::forward<_Str>(path), mode)) {}
	template<typename _Ins>
	instance(std::enable_if_t<std::is_same_v<instance, _Ins>, _Ins>&& other) : _Mybase(std::forward<_Ins>(other)) {}
	virtual ~instance() {}

	decltype(auto) operator<<(const _Elem* str) const noexcept
	{
		if constexpr (std::is_base_of_v<std::basic_ostream<_Elem, std::char_traits<_Elem>>, _Stream>)
			**this << str;
		else
			assert("no matched operator<<()");
		return **this;
	}
	decltype(auto) operator>>(_Elem* str) const noexcept
	{
		if constexpr (std::is_base_of_v<std::basic_istream<_Elem, std::char_traits<_Elem>>, _Stream>)
			**this >> str;
		else
			assert("no matched operator>>()");
		return **this;
	}
	template<typename _Right>
	decltype(auto) operator<<(const _Right& value) const noexcept
	{
		static_assert(std::is_base_of_v<std::basic_ostream<_Elem, std::char_traits<_Elem>>, _Stream>, "no matched operator<<()");
		**this << value;
		return **this;
	}
	template<typename _Right>
	decltype(auto) operator>>(_Right& value) const noexcept
	{
		static_assert(std::is_base_of_v<std::basic_istream<_Elem, std::char_traits<_Elem>>, _Stream>, "no matched operator>>()");
		**this >> value;
		return **this;
	}
};
template<typename _Elem>
class unpack_lines_from_file :public any_class
{
public:
	std::basic_ifstream< _Elem> stream;
	_Elem* buffer;
	size_t size;
	unpack_lines_from_file(const _Elem* path_, std::ios::openmode mode, _Elem* buffer, size_t size) :
		stream(path_, mode), __init(buffer), __init(size) {}
	virtual ~unpack_lines_from_file() {}

	struct iter
	{
		unpack_lines_from_file* parent;
		iter(unpack_lines_from_file* parent) :__init(parent) {}

		auto& operator++(void)
		{
			return *this;
		}

		auto operator*() const
		{
			return this->parent->buffer;
		}

		bool operator!=(const iter& rhs) const
		{
			bool result = static_cast<bool>(this->parent->stream);
			parent->stream.getline(parent->buffer, parent->size);
			return result;
		}
	};

	auto begin()
	{
		return iter(this);
	}
	auto end()
	{
		return iter(this);
	}

};

#endif // !__FILE_CONVENTION_IOSTREAM_INSTANCE
