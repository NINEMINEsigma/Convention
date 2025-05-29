#ifndef CONVENTION_KIT_STREAM_INSTANCE_H
#define CONVENTION_KIT_STREAM_INSTANCE_H

#include "Convention/instance/Interface.h"

template<>
class instance<std::ios_base, true> :public instance<std::ios_base, false>
{
	using TMybase = instance<std::ios_base, false>;
public:
	explicit instance(_shared& rv) :TMybase(rv) {}
	explicit instance(_shared&& rv) :TMybase(std::move(rv)) {}
	template<typename... TArgs>
	instance(TArgs&&... args) :TMybase(std::forward<TArgs>(args)...) {}
	instance& operator=(const instance& other) noexcept
	{
		TMybase::operator=(other);
		return *this;
	}
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

	template<typename TRight>
	decltype(auto) operator<<(TRight&& value)
	{
		dynamic_cast<std::ostream&>(**this) << value;
		return **this;
	}
	template<typename TRight>
	decltype(auto) operator>>(TRight& value)
	{
		dynamic_cast<std::istream&>(**this) >> value;
		return **this;
	}
};
template<template<typename,typename> class TStreamType, typename TElement>
class instance<TStreamType<TElement, std::char_traits<TElement>>, true> :public instance<TStreamType<TElement, std::char_traits<TElement>>, false>
{
public:
	using TStream = TStreamType<TElement, std::char_traits<TElement>>;
	using TMybase = instance<TStream, false>;

	template<typename TString>
	instance(TString&& path, std::ios::openmode mode) : TMybase(new TStream(std::forward<TString>(path), mode)) {}
	template<typename TInstance>
	instance(std::enable_if_t<std::is_same_v<instance, TInstance>, TInstance>&& other) : TMybase(std::forward<TInstance>(other)) {}
	virtual ~instance() {}

	decltype(auto) operator<<(const TElement* str) const noexcept
	{
		if constexpr (std::is_base_of_v<std::basic_ostream<TElement, std::char_traits<TElement>>, TStream>)
			**this << str;
		else
			assert("no matched operator<<()");
		return **this;
	}
	decltype(auto) operator>>(TElement* str) const noexcept
	{
		if constexpr (std::is_base_of_v<std::basic_istream<TElement, std::char_traits<TElement>>, TStream>)
			**this >> str;
		else
			assert("no matched operator>>()");
		return **this;
	}
	template<typename TRight>
	decltype(auto) operator<<(const TRight& value) const noexcept
	{
		static_assert(std::is_base_of_v<std::basic_ostream<TElement, std::char_traits<TElement>>, TStream>, "no matched operator<<()");
		**this << value;
		return **this;
	}
	template<typename TRight>
	decltype(auto) operator>>(TRight& value) const noexcept
	{
		static_assert(std::is_base_of_v<std::basic_istream<TElement, std::char_traits<TElement>>, TStream>, "no matched operator>>()");
		**this >> value;
		return **this;
	}
};
template<typename TElement>
class UnpackLinesFromFile :public any_class
{
public:
	std::basic_ifstream<TElement> stream;
	TElement* buffer;
	size_t size;

	UnpackLinesFromFile(const TElement* path, std::ios::openmode mode, TElement* buffer, size_t size) :
		stream(path, mode), buffer(buffer), size(size) {}
	virtual ~UnpackLinesFromFile() {}

	struct Iterator
	{
		UnpackLinesFromFile* parent;
		Iterator(UnpackLinesFromFile* parent) :parent(parent) {}

		auto& operator++(void)
		{
			return *this;
		}

		auto operator*() const
		{
			return this->parent->buffer;
		}

		bool operator!=(const Iterator& rhs) const
		{
			bool result = static_cast<bool>(this->parent->stream);
			parent->stream.getline(parent->buffer, parent->size);
			return result;
		}
	};

	auto begin()
	{
		return Iterator(this);
	}
	auto end()
	{
		return Iterator(this);
	}
};

#endif // !CONVENTION_KIT_STREAM_INSTANCE_H
