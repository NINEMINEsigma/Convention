#ifndef __FILE_CONVENTION_PROCESS_INSTANCER
#define __FILE_CONVENTION_PROCESS_INSTANCER

#include "Convention/Interface.h"

struct process_indicator
{
	using tag = void;
	static constexpr bool value = true;
};
#if defined(_WINDOWS)
#include<Windows.h>
#else

#endif
template<>
class instance<process_indicator, true> :public any_class
{
#if defined(_WINDOWS)
	using start_info_type = STARTUPINFO;
	using process_info_type = PROCESS_INFORMATION;
#else
	using start_info_type = int;
	using process_info_type = int;
#endif // _WINDOW_
private:
	start_info_type start_info;
	process_info_type pinfo;
	bool stats;
public:
	using config_type = int;

	using tag = void;
	constexpr static bool value = true;

	instance();
	instance(const char_indicator::tag* commandline, const config_type & config = 0);
	instance(const char_indicator::tag* executer, const char_indicator::tag* commandline_args, const config_type & config = 0);
	instance(const instance&) = delete;
	virtual ~instance();
	bool get_stats() const noexcept;

	virtual bool exc(const char_indicator::tag* commandline, const config_type& config = 0);
	virtual bool exc(const char_indicator::tag* executer, const char_indicator::tag* commandline_args, const config_type& config = 0);

	virtual bool kill();
	bool is_still_alive();

	bool when_destructor_kill_but_not_wait = false;
};

#endif // !__FILE_CONVENTION_PROCESS_INSTANCER
