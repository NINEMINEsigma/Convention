#ifndef __FILE_CONVENTION_FFMPEG_INSTANCE
#define __FILE_CONVENTION_FFMPEG_INSTANCE

#include "Convention/Interface.h"
#include "Convention/process_instance.h"

struct ffmpeg_indicator
{
	using tag = void;
	static constexpr bool value = true;
};
template<>
class instance<ffmpeg_indicator, true> :public instance<process_indicator, true>
{
private:
	auto inject_get_quiet_command(int showmode)
	{
		return make_string("ffplay -autoexit -v quiet -showmode ") +
			string_indicator::to_string(showmode) + make_string(" ");
	}
	auto inject_get_command(int showmode)
	{
		return make_string("ffplay -autoexit -showmode ") +
			string_indicator::to_string(showmode) + make_string(" ");
	}
public:
	using _MyBase = instance<process_indicator, true>;
	template<typename... _Args>
	instance(_Args&&... args) :_MyBase(std::forward<_Args>(args)...) {}
	virtual ~instance() {}

	bool play(const string_indicator::tag& target, int showmode = 0)
	{
		auto command = this->inject_get_command(showmode) + make_string("\"") + target + make_string("\"");
		return this->exc(command.c_str());
	}
	bool play_quiet(const string_indicator::tag& target, int showmode = 0)
	{
		auto command = this->inject_get_quiet_command(showmode) + make_string("\"") + target + make_string("\"");
		return this->exc(command.c_str());
	}
	bool transform(
		const string_indicator::tag& input,
		const string_indicator::tag& output
	)
	{
		return this->exc((
			make_string("ffmpeg -v quiet -i") + input + make_string(" ") + output
			).c_str());
	}
};

#endif // !__FILE_CONVENTION_FFMPEG_INSTANCE
