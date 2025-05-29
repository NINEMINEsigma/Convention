#ifndef CONVENTION_KIT_APP_INSTANCE_H
#define CONVENTION_KIT_APP_INSTANCE_H

#include "Convention/instance/Interface.h"
#include "Convention/instance/file_instance.h"

#ifdef _WINDOWS
#include <Windows.h>
#include <shlobj.h>
#endif // _WINDOWS

#ifndef CURRENT_COM_NAME
// set current-com-name to control platform paths
#define CURRENT_COM_NAME "com.default"
#endif // !CURRENT_COM_NAME


#ifndef CURRENT_APP_NAME
// set current-app-name to control platform paths
#define CURRENT_APP_NAME "unname"
#endif // CURRENT_PROJECT_NAME

template<>
class instance<platform_indicator, true> :public any_class
{
private:
	std::filesystem::path InjectPersistentPath() const
	{
		std::filesystem::path result;
		auto p = ::getenv("HOME");
#ifdef _WINDOWS
		if (p)
			result = std::filesystem::path(p);
		else
		{
			PWSTR pathTmp;
			auto getFolderPathRet = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathTmp);
			/* Error check */
			if (getFolderPathRet != S_OK)
			{
				CoTaskMemFree(pathTmp);
				return std::filesystem::path(CNTEXT(CURRENT_COM_NAME)) / CNTEXT(CURRENT_APP_NAME);
			}
			std::filesystem::path path = pathTmp;
			CoTaskMemFree(pathTmp);
			result = path / CNTEXT(CURRENT_COM_NAME) / CNTEXT(CURRENT_APP_NAME);
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

	using StringValueType = const string_indicator::tag&;

	StringValueType Platform() const noexcept
	{
		static auto str = MakeString(__PLATFORM_NAME);
		return str;
	}
	StringValueType PlatformVersion() const noexcept
	{
		static auto str = MakeString(__PLATFORM_VERSION);
		return str;
	}
	StringValueType Extension() const noexcept
	{
		static auto str = MakeString(__PLATFORM_EXTENSION);
		return str;
	}
	const std::filesystem::path& ApplicationPath() const noexcept
	{
		static auto path = std::filesystem::current_path();
		return path;
	}
	const std::filesystem::path& StreamingAssetsPath() const noexcept
	{
		static auto path = ApplicationPath() / "StreamingAssets/";
		return path;
	}
	const std::filesystem::path& PersistentPath() const noexcept
	{
		static auto path = this->InjectPersistentPath();
		return path;
	}

};

#endif // !CONVENTION_KIT_APP_INSTANCE_H
