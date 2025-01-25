#include "Convention.h"
#ifdef _WINDOWS
#include <conio.h>
#include<Windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif


using namespace std;

std::string platform_indicator::generate_platform_message() noexcept
{
	return std::string("Platform: ") + __PLATFORM_NAME + "-" + __PLATFORM_VERSION + "-" + __PLATFORM_EXTENSION;
}
bool platform_indicator::keyboard_input(size_t key) noexcept
{
#ifdef _WINDOWS
	if (_kbhit())
		return _getch() == key || key == 0;
#else
	fd_set rfds;
	struct timeval tv;

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = 1; //设置等待超时时间
	if (select(1, &rfds, NULL, NULL, &tv) > 0)
		return key == getchar() || key == 0;
#endif // _WINDOWS
	return false;
}

size_t string_indicator::strlen(const char_indicator::tag* str)
{
#ifdef UNICODE
	return ::wcslen(str);
#else
	return ::strlen(str);
#endif // UNICODE
}
char_indicator::tag* string_indicator::strcpy(
	char_indicator::tag* dest,
	const char_indicator::tag* source
)
{
#ifdef UNICODE
	return ::wcscpy(dest, source);
#else
	return ::strcpy(dest, source);
#endif // UNICODE
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
	_Ret_maybenull_ void* find_target_flag_class_ptr(
		_In_reads_bytes_(length) const void* head, size_t length,
		_In_reads_bytes_(flag_length) const void* target_flag, size_t flag_length,
		size_t flag_offset
	)
	{
		for (size_t i = 0; i != length; i++)
		{
			if (0 == memcmp(reinterpret_cast<void*>(reinterpret_cast<size_t>(head) + i), target_flag, flag_length))
			{
				return reinterpret_cast<void*>(reinterpret_cast<size_t>(head) + i - flag_offset);
			}
		}
		return nullptr;
	}
	_Ret_maybenull_ any_class* find_any_class_ptr(
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

std::map<std::string, std::string> make_config(int argv, char** argc)
{
	std::map<std::string, std::string> result;
	std::string key;
	std::string value;
	bool is_key = true;
	for (int i = 1; i < argv; i++)
	{
		if (argc[i][0] == '-')
		{
			if (is_key)
				key = argc[i];
			else
				result[key] = std::move(value);
			is_key = false;
			key = argc[i];
			while (key.front() == '-')
			{
				key.erase(key.begin());
				if (key.size() == 0)
				{
					is_key = true;
					break;
				}
			}
		}
		else if (is_key==false)
		{
			result[std::move(key)] = argc[i];
			is_key = true;
		}
		else
		{
			result[argc[i]] = "";
			is_key = true;
		}
	}
	return result;
}

// file_instance.h
bool is_binary_file(const std::filesystem::path& path)
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
std::filesystem::path get_extension_name(const std::filesystem::path& path)
{
	return path.extension();
}
std::filesystem::path get_base_filename(const std::filesystem::path& path)
{
	return path.filename();
}


