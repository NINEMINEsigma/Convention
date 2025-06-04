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

		template<typename Stream,typename Arg>
		instance& operator<<(const Arg& value)
		{
			this->ReadStreamValue<Stream>() << value;
			return *this;	
		}
	};

	template<typename CharTy, typename CharTrait = std::char_traits<CharTy>>
	class UnpackLinesFromFile
		: public AnyClass
	{
	private:
		std::basic_ifstream<CharTy, CharTrait> stream;
		CharTy* buffer;
		size_t size;
		bool isGenerateBuffer;
	public:
		UnpackLinesFromFile(
			_In_ const CharTy* path,
			std::ios::openmode mode,
			_In_reads_opt_(size) CharTy* buffer,
			size_t size)
			: isGenerateBuffer(buffer == nullptr), stream(path, mode), buffer(buffer ? buffer : new CharTy[size]), size(size)
		{
		}
		virtual ~UnpackLinesFromFile()
		{
			if (isGenerateBuffer)
			{
				delete[] buffer;
			}
		}

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
