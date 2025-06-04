#include "Convention/Config.h"
#if defined(_WIN64)||defined(_WIN32)
#include <conio.h>
#include<Windows.h>
#include <shlobj.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif

using namespace std;

int PlatformIndicator::KeyboardInput() noexcept
{
#ifdef _WINDOWS
	if (_kbhit())
		return _getch();
#else
	fd_set rfds;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 1;
	if (select(1, &rfds, NULL, NULL, &tv) > 0)
		return getchar();
#endif // _WINDOWS
	return -1;
}

std::filesystem::path PlatformIndicator::InjectPersistentPath()
{
	std::filesystem::path result;
	auto p = ::getenv("HOME");
	if constexpr (PlatformIndicator::IsPlatformWindows)
	{
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
				return std::filesystem::path(CURRENT_COM_NAME) / CURRENT_APP_NAME;
			}
			std::filesystem::path path = pathTmp;
			CoTaskMemFree(pathTmp);
			result = path / CURRENT_COM_NAME / CURRENT_APP_NAME;
		}
	}
	else
	{
		result = p;
	}
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

// file_instance.h
bool IsBinaryFile(const std::filesystem::path& path)
{
	std::ifstream fs(path, std::ios::in | std::ios::binary);
	void* buffer = no_warning_6387(malloc(sizeof(char)));
	void* checker = no_warning_6387(malloc(sizeof(char)));
	memset(buffer, 0, sizeof(char));
	memset(checker, 0, sizeof(char));
	for (int i = 1024; i != 0 && fs; i--)
	{
		fs.read((char*)buffer, sizeof(char));
		if (memcmp(buffer, checker, sizeof(char)))
			return true;
	}
	return false;
}
std::filesystem::path GetExtensionName(const std::filesystem::path& path)
{
	return path.extension();
}
std::filesystem::path GetBaseFilename(const std::filesystem::path& path)
{
	return path.filename();
}
std::vector<std::string> tool_file::text_readable_file_type = { "txt", "md", "json", "csv", "xml", "xlsx", "xls", "docx", "doc", "svg" };
std::vector<std::string> tool_file::audio_file_type = { "mp3", "ogg", "wav" };
std::vector<std::string> tool_file::image_file_type = { "'png", "jpg", "jpeg", "bmp", "svg", "ico" };
std::string tool_file::temp_tool_file_path_name = "temp.tool_file";


