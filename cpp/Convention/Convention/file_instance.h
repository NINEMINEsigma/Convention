#ifndef __FILE_CONVENTION_FILE_INSTANCE
#define __FILE_CONVENTION_FILE_INSTANCE

#include "Convention/Interface.h"
#include "filesystem"

template<>
class instance<std::filesystem::path, true> :public instance<std::filesystem::path, false>
{
public:
	using buffer_type = std::basic_string<size_t>;

	using path = std::filesystem::path;
	using _MyBase = instance<path, false>;

	instance() = delete;
	instance(nullptr_t) = delete;
	explicit instance(path path_) :_MyBase(new path(path_)) {}
	explicit instance(const char* path_) : _MyBase(new path(path_)) {}
	explicit instance(const wchar_t* path_) : _MyBase(new path(path_)) {}
	instance(const std::wstring& path_) :_MyBase(new path(path_)) {}

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

	void open(path path_)
	{
		**this = std::move(path_);
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
	void must_exist_path() const noexcept
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
	auto get_stream(std::ios::openmode mode) const
	{
		return std::fstream(**this, mode);
	}
	auto get_wstream(std::ios::openmode mode) const
	{
		return std::wfstream(**this, mode);
	}


private:

};

#endif // !__FILE_CONVENTION_FILE_INSTANCE
