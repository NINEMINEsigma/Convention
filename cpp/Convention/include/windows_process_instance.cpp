#ifdef _WINDOWS

#include "Convention\process_instance.h"

using namespace std;
using process_instance = instance<process_indicator, true>;

process_instance::instance() :stats(false), start_info({}), pinfo({}) {}
process_instance::instance(
	const char_indicator::tag* commandline,
	const config_type& config
) :instance()
{
	this->exc(commandline, config);
}
bool process_instance::exc(
	const char_indicator::tag* commandline,
	const config_type& config
)
{
	memset(&this->start_info, 0, sizeof(this->start_info));
	char_indicator::tag commandline_c[255] = { 0 };
	string_indicator::strcpy(commandline_c, commandline);
	this->start_info.cb = sizeof(this->start_info);
	if (CreateProcess(
		0,
		commandline_c,
		0,
		0,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		0,
		0,
		&this->start_info,
		&this->pinfo))
	{
		this->stats = true;
	}
	else
		this->stats = false;
	return this->stats;
}
process_instance::instance(
	const char_indicator::tag* executer,
	const char_indicator::tag* commandline_args,
	const config_type& config) :instance()
{
	this->exc(executer, commandline_args, config);
}
bool process_instance::exc(
	const char_indicator::tag* executer,
	const char_indicator::tag* commandline_args,
	const config_type& config)
{
	memset(&this->start_info, 0, sizeof(this->start_info));
	this->start_info.cb = sizeof(this->start_info);
	char_indicator::tag cammand_c_args[255] = { 0 };
	string_indicator::strcpy(cammand_c_args, commandline_args);
	if (CreateProcess(
		executer,
		cammand_c_args,
		0,
		0,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		0,
		0,
		&this->start_info,
		&this->pinfo))
	{
		this->stats = true;
	}
	else
		this->stats = false;
	return this->stats;
}
process_instance::instance(process_indicator) :instance() {}
process_instance::~instance()
{
	if (this->stats)
	{
		WaitForSingleObject(this->pinfo.hProcess, INFINITE);
		CloseHandle(this->pinfo.hProcess);
		CloseHandle(this->pinfo.hThread);
	}
}
bool process_instance::get_stats() const noexcept
{
	return stats;
}

bool process_instance::kill()
{
	if (this->stats)
	{
		TerminateProcess(this->pinfo.hProcess, 0);
		CloseHandle(this->pinfo.hProcess);
		CloseHandle(this->pinfo.hThread);
		this->stats = false;
		return true;
	}
	return false;
}
bool process_instance::is_still_alive()
{
	if (this->stats)
	{
		DWORD code;
		if (GetExitCodeProcess(this->pinfo.hProcess, &code)==TRUE)
		{
			bool result = code == STILL_ACTIVE;
			this->stats = result;
			return result;
		}
	}
	return false;
}

#endif // _WINDOWS
