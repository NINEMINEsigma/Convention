#ifndef __FILE_CONVENTION_FILE_INSTANCE
#define __FILE_CONVENTION_FILE_INSTANCE

#include "Convention/Interface.h"
#include <filesystem>
#include "Convention/stream_instance.h"

extern bool is_binary_file(const std::filesystem::path& path);
extern std::filesystem::path get_extension_name(const std::filesystem::path& path);
extern std::filesystem::path get_base_filename(const std::filesystem::path& path);

template<>
class instance<std::filesystem::path, true> :public instance<std::filesystem::path, false>
{
public:
	using buffer_type = std::basic_string<size_t>;

	using path = std::filesystem::path;
	using _Stream = instance<std::ios_base, true>;
private:
	using _Mybase = instance<path, false>;
public:
	_Stream stream = nullptr;

	instance() = delete;
	instance(nullptr_t) = delete;
	instance(const _shared& shared_path, bool is_must_exist=true) :_Mybase(shared_path)
	{
		if (is_must_exist)
			this->must_exist_path();
	}
	instance(path path_) :_Mybase(new path(path_)) {}
	instance_move_operator(public)
	{
		this->stream = std::move(other.stream);
	}
	virtual ~instance() {}

	path get_filename(bool is_without_extension = false) const
	{
		std::string cur = this->get()->filename().string();
		if (is_without_extension && this->get()->has_extension())
		{
			return cur.substr(0, cur.find_last_of('.'));
		}
		else if (cur.back() == '\\' || cur.back() == '/')
			return cur.substr(0, cur.size() - 1);
		return cur;
	}
	virtual std::string ToString() const noexcept override
	{
		return this->get_filename().string();
	}
	virtual std::string SymbolName() const noexcept override
	{
		return Combine(
			"file<",
			this->exist()?"e":"-",
			this->is_dir() ? "d" : "-",
			">"
		);
	}

	// get target path's stats

	bool is_dir() const noexcept
	{
		auto endchar = this->get()->string().back();
		if (endchar == '/' || endchar == '\\')
			return true;
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
	bool is_binary_file() const
	{
		return ::is_binary_file(**this);
	}

	// operators

	instance& open(path path_)
	{
		**this = std::move(path_);
		return *this;
	}
	instance& open(std::ios::openmode mode)
	{
		auto* ptr = new std::fstream(**this, mode);
		this->stream = ptr;
		return *this;
	}
	instance& close()
	{
		if (this->stream.is_empty() == false)
		{
			this->stream = nullptr;
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
	bool is_path_empty() const noexcept
	{
		return this->get()->empty();
	}
	instance& rename(path new_name)
	{
		std::filesystem::rename(**this, new_name);
		**this = new_name;
		return *this;
	}
	void copy_c(const path& target) const
	{
		if (target.has_root_directory())
		{
			std::filesystem::copy(**this, target);
		}
		else
		{
			path cur = this->get()->parent_path() / target;
			std::filesystem::copy(**this, cur);
		}
	}
	instance& copy(const path& target)
	{
		this->copy_c(target);
		return *this;
	}
	instance& move(const path& target)
	{
		if (target.has_root_directory())
		{
			std::filesystem::copy(**this, target);
			std::filesystem::remove(**this);
			**this = target;
		}
		else
		{
			path cur = this->get()->parent_path() / target;
			std::filesystem::copy(**this, cur);
			std::filesystem::remove(**this);
			**this = cur;
		}
		return *this;
	}
	void delete_c() const
	{
		if (this->is_dir())
			std::filesystem::remove_all(**this);
		else
			std::filesystem::remove(**this);
	}
	instance& remove()
	{
		std::filesystem::remove(**this);
		return *this;
	}
	instance& remove_all()
	{
		std::filesystem::remove_all(**this);
		return *this;
	}
	instance& must_exist_path() noexcept
	{
		this->stream = nullptr;
		this->try_create_parent_path_c();
		this->create();
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

	void try_create_parent_path_c() const
	{
		path dir = **this;
		if (dir.has_parent_path())
		{
			dir = dir.parent_path();
			std::filesystem::create_directories(dir);
		}
	}
	instance& try_create_parent_path()
	{
		this->try_create_parent_path_c();
		return *this;
	}
	auto dir_iter() const
	{
		return std::filesystem::directory_iterator(**this);
	}
	std::vector<instance> dir_instance_iter() const
	{
		std::vector<instance> result;
		for (auto&& index : this->dir_iter())
		{
			result.push_back(instance(index.path()));
		}
		return result;
	}
	instance& back_to_parent_dir()
	{
		**this = this->get()->parent_path();
		return *this;
	}
	size_t dir_count() const
	{
		size_t result = 0;
		for (auto&& _ : this->dir_iter())
			result++;
		return result;
	}
	void dir_clear_c() const
	{
		for (auto&& index : this->dir_iter())
			std::filesystem::remove(index);
	}
	instance& dir_clear()
	{
		this->dir_clear_c();
		return *this;
	}
	// if found return it, otherwise return myself
	instance first_file_with_extension(const std::string& extension) const
	{
		for (auto&& index : this->dir_iter())
			if (index.path().extension() == extension)
				return instance(index.path());
		return instance(static_cast<_shared>(*this));
	}
	// if found return it, otherwise return myself
	instance first_file_with_name(const std::string& name) const
	{
		for (auto&& index : this->dir_iter())
		{
			auto cur = index.path();
			if (cur.has_filename() == false)continue;
			auto sv = cur.filename().string();
			if (name.size() <= sv.size() && 0 == decltype(sv)::traits_type::compare(sv.data(), name.data(), name.size()))
				return instance(cur);
		}
		return instance(static_cast<_shared>(*this));
	}
	// if found return it, otherwise return myself
	instance first_file(std::function<bool(const path&)> pr) const
	{
		for (auto&& index : this->dir_iter())
			if (pr(index))
				return instance(index.path());
		return instance(static_cast<_shared>(*this));
	}
	// if found return it, otherwise return myself
	instance first_file(std::function<bool(const instance&)> pr) const
	{
		for (auto&& index : this->dir_instance_iter())
			if (pr(index))
				return instance(std::move(index));
		return instance(static_cast<_shared>(*this));
	}
	instance& make_file_inside(instance& data, bool is_delete_source = false)
	{
		if (this->is_dir() == false)
			throw std::filesystem::filesystem_error(
				"Cannot make file inside a file, because this object target is not a directory",
				**this,
				std::make_error_code(std::errc::is_a_directory)
			);
		auto result = *this | data.get_filename();
		if (is_delete_source)
			data.move(result.get_filename());
		else
			data.copy(result.get_filename());
		return *this;
	}

	// operators

	instance operator|(const path& next) const noexcept
	{
		return instance(**this / next);
	}
	instance operator|(nullptr_t) const
	{
		if (this->is_dir())
			return instance(static_cast<_shared>(*this));
		return instance(this->get()->string() + "/");
	}
	instance operator|(_shared ptr) const
	{
		return instance(**this / (*ptr));
	}

	// stream operators

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
	template<typename _StreamType>
	decltype(auto) get_stream() const
	{
		if constexpr (std::is_pointer_v<_StreamType>)
			return dynamic_cast<_StreamType>(this->stream.get());
		else if constexpr (std::is_lvalue_reference_v<_StreamType>)
			return dynamic_cast<_StreamType>(*this->stream);
		else if constexpr (std::is_rvalue_reference_v<_StreamType>)
			return dynamic_cast<_StreamType>(std::move(*this->stream));
		else
			return dynamic_cast<_StreamType&>(*this->stream);
	}
	template<typename... _Args>
	decltype(auto) set_stream(_Args... args)
	{
		this->stream = _Stream(std::forward<_Args>(args)...);
		return *this;
	}
	

private:

};

#endif // !__FILE_CONVENTION_FILE_INSTANCE
