#ifndef __FILE_CONVENTION_FILE_INSTANCE
#define __FILE_CONVENTION_FILE_INSTANCE

#include "Convention/Interface.h"
#include "filesystem"
#include "Convention/iostream_instance.h"

template<>
class instance<std::filesystem::path, true> :public instance<std::filesystem::path, false>
{
public:
	using buffer_type = std::basic_string<size_t>;

	using path = std::filesystem::path;
	using _MyBase = instance<path, false>;
	using _Stream = instance<std::ios_base, true>;

	_Stream stream = nullptr;
	std::ios::openmode stream_mode = 0;

	instance() = delete;
	instance(nullptr_t) = delete;
	explicit instance(_shared& rv) :_MyBase(rv) {}
	explicit instance(_shared&& rv) :_MyBase(std::move(rv)) {}
	explicit instance(path path_) :_MyBase(new path(path_)) {}
	explicit instance(const char* path_) : _MyBase(new path(path_)) {}
	explicit instance(const wchar_t* path_) : _MyBase(new path(path_)) {}
	instance(const std::wstring& path_) :_MyBase(new path(path_)) {}
	explicit instance(instance&& move_) noexcept:
		_MyBase(std::move(move_)),
		stream(std::move(move_.stream)),
		stream_mode(move_.stream_mode) {}
	virtual ~instance() {}

	// get stats

	bool is_dir() const noexcept
	{
		return std::filesystem::is_directory(**this);
	}
	bool is_file() const noexcept
	{
		return !this->is_dir();
	}
	bool is_block_file() const noexcept
	{
		return std::filesystem::is_block_file(**this);
	}
	bool is_character_file() const noexcept
	{
		return std::filesystem::is_character_file(**this);
	}
	bool is_file_empty() const noexcept
	{
		return std::filesystem::is_empty(**this);
	}

	// operators

	auto& open(path path_)
	{
		**this = std::move(path_);
		return *this;
	}
	auto& open(std::ios::openmode mode)
	{
		if (this->stream_mode = mode)
		{
			this->stream->setstate(std::ios::beg);
			this->stream->clear();
		}
		else
		{
			stream_mode = mode;
			auto* ptr = new std::fstream(**this, mode);
			this->stream = ptr;
		}
		return *this;
	}
	bool exist() const noexcept
	{
		return std::filesystem::exists(**this);
	}
	void create() const noexcept
	{
		if (this->exist())
			return;
		if (this->is_dir())
			std::filesystem::create_directory(**this);
		else
		{
			std::ofstream of(**this, std::ios::out);
		}
	}
	void must_exist_path_c() const noexcept
	{
		if (this->exist())
			return;
		if (this->is_dir())
			std::filesystem::create_directories(**this);
		else
		{
			std::ofstream of(**this, std::ios::out);
		}
	}
	auto& must_exist_path() noexcept
	{
		this->must_exist_path_c();
		return *this;
	}
	auto get_stream(std::ios::openmode mode) const
	{
		return std::fstream(**this, mode);
	}
	auto get_wstream(std::ios::openmode mode) const
	{
		return std::wfstream(**this, mode);
	}

	template<typename _Type>
	decltype(auto) operator<<(const _Type& value)
	{
		this->stream << value;
		return this->stream;
	}
	template<typename _Type>
	decltype(auto) operator>>(_Type& value)
	{
		this->stream >> value;
		return this->stream;
	}

private:

};

#endif // !__FILE_CONVENTION_FILE_INSTANCE
