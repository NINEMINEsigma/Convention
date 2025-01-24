#ifndef __FILE_CONVENTION_IOSTREAM_INSTANCE
#define __FILE_CONVENTION_IOSTREAM_INSTANCE

#include "Convention\Interface.h"

template<>
class instance<std::ios_base, true> :public instance<std::ios_base, false>
{
public:
	using _MyBase = instance< std::ios_base, false>;
	explicit instance(_shared& rv) :_MyBase(rv) {}
	explicit instance(_shared&& rv) :_MyBase(std::move(rv)) {}
	template<typename... _Args>
	instance(_Args&&... args) :_MyBase(std::forward<_Args>(args)...) {}
	virtual ~instance() {}

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
/*
template<class _Stream>
class instance<_Stream, true> :public std::enable_if_t <
	internal::is_stream_v<_Stream> && !internal::is_number_v<_Stream>,
	instance< _Stream, false>
>
{
public:
	using _MyBase = instance< _Stream, false>;
	explicit instance(_Stream* ptr) :_MyBase(ptr) {}
	explicit instance(_shared& rv) :_MyBase(rv) {}
	explicit instance(_shared&& rv) :_MyBase(std::move(rv)) {}
	template<typename _Str>
	instance(_Str&& path, std::ios::openmode mode) : _MyBase(new _Stream(std::forward<_Str>(path), mode)) {}
	template<typename _Ins>
	instance(std::enable_if_t<std::is_same_v<instance, _Ins>, _Ins>&& other) : _MyBase(std::forward<_Ins>(other)) {}
	virtuao ~instance() {}

	template<typename _Right>
	decltype(auto) operator<<(std::enable_if_t<std::is_base_of_v<std::basic_ostream, _Stream>, _Right>&& value)
	{
		**this << value;
		return **this;
	}
	template<typename _Right>
	decltype(auto) operator>>(std::enable_if_t<std::is_base_of_v<std::basic_istream, _Stream>, _Right>& value)
	{
		**this >> value;
		return **this;
	}
};
*/

template<typename _Elem>
class istream_line_range :public any_class
{
public:
	std::basic_ifstream< _Elem> stream;
	_Elem* buffer;
	size_t size;
	istream_line_range(const _Elem* path_, std::ios::openmode mode, _Elem* buffer, size_t size) :
		stream(path_, mode), __init(buffer), __init(size) {}
	virtual ~istream_line_range() {}

	struct iter
	{
		istream_line_range* parent;
		iter(istream_line_range* parent) :__init(parent) {}

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
