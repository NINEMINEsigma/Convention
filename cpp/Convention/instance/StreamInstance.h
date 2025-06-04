#ifndef CONVENTION_KIT_STREAM_INSTANCE_H
#define CONVENTION_KIT_STREAM_INSTANCE_H

#include "Convention/instance/Interface.h"

namespace Convention
{
	template<template<typename...> class Allocator>
	class instance<std::ios_base, true, Allocator, false>
		: public instance<std::ios_base, false, Allocator, false>
	{
	private:
		using _Mybase = instance<std::ios_base, false, Allocator, false>;
	public:
		instance(const instance& other) noexcept : _Mybase(other) {}
		instance(instance&& other) noexcept : _Mybase(std::move(other)) {}
		template<typename... Args>
		instance(Args&&... args) : _Mybase(BuildMyPtr(std::forward<Args>(args)...)) {}
		instance& operator=(const instance& other) noexcept
		{
			_Mybase::operator=(other);
			return *this;
		}
		instance& operator=(instance&& other) noexcept
		{
			_Mybase::operator=(std::move(other));
			return *this;
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

		template<typename Stream>
		bool ReadStreamValue(_Out_ Stream* ptr) const
		{
			return ptr = dynamic_cast<Stream*>(this->get());
		}
		template<typename Stream>
		const Stream& ReadStreamValue() const
		{
			return *dynamic_cast<Stream*>(this->get());
		}
	};

	template<template<typename, typename> class TStreamType, typename TElement, template<typename...> class Allocator>
	class instance<TStreamType<TElement, std::char_traits<TElement>>, true, Allocator, false>
		: public instance<TStreamType<TElement, std::char_traits<TElement>>, false, Allocator, false >
	{
	public:
		using TStream = TStreamType<TElement, std::char_traits<TElement>>;
		using _Mybase = instance<TStream, false>;

		template<typename TString>
		instance(TString&& path, std::ios::openmode mode) : _Mybase(new TStream(std::forward<TString>(path), mode)) {}
		template<typename TInstance>
		instance(std::enable_if_t<std::is_same_v<instance, TInstance>, TInstance>&& other) : _Mybase(std::forward<TInstance>(other)) {}
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
			stream(path, mode), buffer(buffer), size(size) {
		}
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
}

#endif // !CONVENTION_KIT_STREAM_INSTANCE_H
