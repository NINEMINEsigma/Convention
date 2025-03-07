#ifndef __FILE_CONVENTION_FFMPEG_INSTANCE
#define __FILE_CONVENTION_FFMPEG_INSTANCE

#include "Convention/Interface.h"
#include "Convention/process_instance.h"

struct ffmpeg_indicator
{
	using tag = string_indicator::tag;
	static constexpr bool value = true;
};
template<>
class instance<ffmpeg_indicator, true> :public instance<process_indicator, true>
{
private:
	using _Mybase = instance<process_indicator, true>;
public:
	string_indicator::tag commandline;
public:
	instance(const string_indicator::tag& commandline = make_string("")) :_Mybase(), __init(commandline) {}
	explicit instance(ffmpeg_indicator) :instance() {}
	instance_move_operator(public)
	{
		this->commandline = std::move(other.commandline);
	}
	virtual ~instance() {}

	bool ffplay(const string_indicator::tag& target)
	{
		return this->exc((
			make_string("ffplay ") + this->commandline + 
			make_string(" \"") + target + make_string("\"")
			).c_str());
	}
	bool ffmpeg(
		const string_indicator::tag& input,
		const string_indicator::tag& output
	)
	{
		return this->exc((
			make_string("ffmpeg ") + this->commandline +
			make_string(" \"") + input + make_string("\"") +
			make_string(" \"") + output + make_string("\"")
			).c_str());
	}
};
#else

#ifndef __DEFINED_FFMPEG_INDICATOR
#define __DEFINED_FFMPEG_INDICATOR
struct ffmpeg_indicator
{
	using tag = string_indicator::tag;
	static constexpr bool value = false;
};
#endif

#endif // !__FILE_CONVENTION_FFMPEG_INSTANCE
