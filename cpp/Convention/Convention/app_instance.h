#ifndef __FILE_CONVENTION_APP_INSTANCE
#define __FILE_CONVENTION_APP_INSTANCE

#include "Convention/Interface.h"
#include "Convention/file_instance.h"

#ifdef _WINDOWS
#include <Windows.h>
#include <shlobj.h>
#endif // _WINDOWS


#ifndef CURRENT_PROJECT_NAME
// set current-project-name to control platform paths
#define CURRENT_PROJECT_NAME "com.default"
#endif // CURRENT_PROJECT_NAME

template<>
class instance<platform_indicator, true> :public any_class
{
private:
	std::filesystem::path inject_persistent_path() const
	{
		std::filesystem::path result;
		auto p = ::getenv("HOME");
#ifdef _WINDOWS
		if (p)
			result = std::filesystem::path(p);
		else
		{
			PWSTR path_tmp;
			auto get_folder_path_ret = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path_tmp);
			/* Error check */
			if (get_folder_path_ret != S_OK)
			{
				CoTaskMemFree(path_tmp);
				return CNTEXT(CURRENT_PROJECT_NAME);
			}
			std::filesystem::path path = path_tmp;
			CoTaskMemFree(path_tmp);
			result = path / CNTEXT(CURRENT_PROJECT_NAME);
		}
#else
		result = p;
#endif // _WINDOWS
		if (false == std::filesystem::exists(result))
		{
			auto str = result.string();
			if (str.back() != '/' && str.back() != '\\')
				result = str + "/";
		}
		else if (std::filesystem::is_directory(result))
		{
			throw std::filesystem::filesystem_error(
				"persistent path target is not a directory",
				result,
				std::make_error_code(std::errc::not_a_directory)
			);
		}
		return result;
	}

public:
	instance(){}
	instance(const instance&) = delete;
	virtual ~instance() {}

	using string_value_type = const string_indicator::tag&;

	string_value_type platform() const noexcept
	{
		static auto str = make_string(__PLATFORM_NAME);
		return str;
	}
	string_value_type platform_version() const noexcept
	{
		static auto str = make_string(__PLATFORM_VERSION);
		return str;
	}
	string_value_type extension() const noexcept
	{
		static auto str = make_string(__PLATFORM_EXTENSION);
		return str;
	}
	const std::filesystem::path& application_path() const noexcept
	{
		static auto path = std::filesystem::current_path();
		return path;
	}
	const std::filesystem::path& streaming_assets_path() const noexcept
	{
		static auto path = application_path() / "StreamingAssets/";
		return path;
	}
	const std::filesystem::path& persistent_path() const noexcept
	{
		static auto path = this->inject_persistent_path();
		return path;
	}
	
};

#endif // !__FILE_CONVENTION_APP_INSTANCE
