#include "Convention/Convention.h"
#ifdef _WINDOWS
#include <conio.h>
#include<Windows.h>
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


