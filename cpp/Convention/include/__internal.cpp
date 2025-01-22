#include "Convention.h"

using namespace std;

std::string platform_indicator::generate_platform_message() noexcept
{
	return std::string("Platform: ") + __PLATFORM_NAME + "-" + __PLATFORM_VERSION + "-" + __PLATFORM_EXTENSION;
}

extern "C"
{
	void __tool_test() noexcept
	{
		std::cout << "tool test: yes" << std::endl;
	}


	_Ret_maybenull_ void* find_target_flag_class_ptr(
		_In_reads_bytes_(length) const void* head, size_t length,
		_In_reads_bytes_(flag_length) const void* target_flag, size_t flag_length,
		size_t flag_offset
	)
	{
		for (size_t i = 1; i != length; i++)
		{
			if (memcmp(reinterpret_cast<void*>(reinterpret_cast<size_t>(head) + i), target_flag, flag_length))
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