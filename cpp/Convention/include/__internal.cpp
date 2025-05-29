#include "Convention/Convention.h"
#ifdef _WINDOWS
#include <conio.h>
#include<Windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif


using namespace std;

std::string platform_indicator::GeneratePlatformMessage() noexcept
{
	return std::string("Platform: ") + __PLATFORM_NAME + "-" + __PLATFORM_VERSION + "-" + __PLATFORM_EXTENSION;
}
int platform_indicator::KeyboardInput() noexcept
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
	tv.tv_usec = 1; //���õȴ���ʱʱ��
	if (select(1, &rfds, NULL, NULL, &tv) > 0)
		return getchar();
#endif // _WINDOWS
	return -1;
}

size_t string_indicator::strlen(const char_indicator::tag* str)
{
	return string_indicator::Traits::length(str);
}
char_indicator::tag* string_indicator::strcpy(
	char_indicator::tag* dest,
	const char_indicator::tag* source
)
{
	return string_indicator::Traits::copy(dest, source, strlen(source));
}
char_indicator::tag* string_indicator::strcpy_s(
	char_indicator::tag* dest,
	const char_indicator::tag* source,
	const size_t size
)
{
	return string_indicator::Traits::copy(dest, source, std::min(size, strlen(source)));
}

size_t string_indicator::c_strlen(const char* str)
{
	return ::strlen(str);
}
char* string_indicator::c_strcpy(
	char* dest,
	const char* source
)
{
	return ::strcpy(dest, source);
}
errno_t string_indicator::c_strcpy_s(
	char* dest,
	const char* source,
	const size_t size
)
{
	return ::strcpy_s(dest, std::min(size, c_strlen(source)), source);
}

std::string typename2classname(const std::string& str) noexcept
{
	if (str.substr(0, 6) == "class ")
		return str.substr(6);
	else if (str.substr(0, 7) == "struct ")
		return str.substr(7);
	return str;
}

extern "C"
{
	_Ret_maybenull_ void* FindTargetFlagClassPtr(
		_In_reads_bytes_(length) const void* head, size_t length,
		_In_reads_bytes_(flagLength) const void* targetFlag, size_t flagLength,
		size_t flagOffset
	)
	{
		for (size_t i = 0; i != length; i++)
		{
			if (0 == memcmp(reinterpret_cast<void*>(reinterpret_cast<size_t>(head) + i), targetFlag, flagLength))
			{
				return reinterpret_cast<void*>(reinterpret_cast<size_t>(head) + i - flagOffset);
			}
		}
		return nullptr;
	}
	_Ret_maybenull_ any_class* FindAnyClassPtr(
		_In_reads_bytes_(length) const void* head, size_t length
	)
	{
		//any_class's flag is not at 0-offset, so we begin in 1 index
		for (size_t i = 1; i != length; i++)
		{
			if (*reinterpret_cast<size_t*>(reinterpret_cast<size_t>(head) + i) == any_class_head_flag)
			{
				return reinterpret_cast<any_class*>(reinterpret_cast<size_t>(head) + i - sizeof(size_t));
			}
		}
		return nullptr;
	}
}

std::tuple<
	std::map<std::string, std::string>,
	std::vector<std::pair<std::string, std::string>>
> MakeConfig(int argc, char** argv)
{
	std::map<std::string, std::string> first;
	std::vector<std::pair<std::string, std::string>> second;
	std::string key;
	std::string value;
	bool isKey = true;
	if (argc > 0)
	{
		first["execute"] = argv[0];
		second.push_back({ argv[0],"" });
	}
	for (int i = 1; i < argc; i++)
	{
		if (second.size() != 0 &&
			second.back().first.front() == '-' &&
			second.back().second.size() == 0 &&
			argv[i][0] != '-'
			)
			second.back().second = argv[i];
		else
			second.push_back({ argv[i],"" });

		if (argv[i][0] == '-')
		{
			if (isKey)
				key = argv[i];
			else
				first[key] = value;
			isKey = false;
			key = argv[i];
			while (key.front() == '-')
			{
				key.erase(key.begin());
				if (key.size() == 0)
				{
					isKey = true;
					break;
				}
			}
		}
		else if (isKey==false)
		{
			first[key] = argv[i];
			isKey = true;
		}
		else
		{
			first[argv[i]] = "";
			isKey = true;
		}
	}
	if (isKey == false)
	{
		first[key] = "";
		second.push_back({ key,"" });
	}
	return make_tuple<
		std::map<std::string, std::string>,
		std::vector<std::pair<std::string, std::string>>
	>(std::move(first),std::move(second));
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

namespace ConventionKit
{
	bool IsArithmeticType(const type_info& type)
	{
		if (
			type == typeid(int) ||
			type == typeid(unsigned int) ||
			type == typeid(long) ||
			type == typeid(unsigned long) ||
			type == typeid(long long) ||
			type == typeid(unsigned long long) ||
			type == typeid(float) ||
			type == typeid(double) ||
			type == typeid(long double))
		{
			return true;
		}
		return false;
	}
	bool IsStringType(const type_info& type)
	{
		if (
			type == typeid(std::string) ||
			type == typeid(std::wstring) ||
			type == typeid(std::u16string) ||
			type == typeid(std::u32string) ||
			type == typeid(std::string_view) ||
			type == typeid(std::wstring_view) ||
			type == typeid(std::u16string_view) ||
			type == typeid(std::u32string_view) ||
			type == typeid(const char*)
			)
		{
			return true;
		}
		return false;
	}
	bool IsFloatingType(const type_info& type)
	{
		if (
			type == typeid(float) ||
			type == typeid(double) ||
			type == typeid(long double))
		{
			return true;
		}
		return false;
	}
	bool IsIntegralType(const type_info& type)
	{
		if (
			type == typeid(int) ||
			type == typeid(unsigned int) ||
			type == typeid(long) ||
			type == typeid(unsigned long) ||
			type == typeid(long long) ||
			type == typeid(unsigned long long))
		{
			return true;
		}
		return false;
	}
	bool IsUnsignedIntegralType(const type_info& type)
	{
		if (
			type == typeid(unsigned int) ||
			type == typeid(unsigned long) ||
			type == typeid(unsigned long long))
		{
			return true;
		}
		return false;
	}
}

std::vector<std::string> tool_file::text_readable_file_type = { "txt", "md", "json", "csv", "xml", "xlsx", "xls", "docx", "doc", "svg" };
std::vector<std::string> tool_file::audio_file_type = { "mp3", "ogg", "wav" };
std::vector<std::string> tool_file::image_file_type = { "'png", "jpg", "jpeg", "bmp", "svg", "ico" };
std::string tool_file::temp_tool_file_path_name = "temp.tool_file";


