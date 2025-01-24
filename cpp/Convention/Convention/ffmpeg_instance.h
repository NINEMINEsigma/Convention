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
public:
	string_indicator::tag commandline;
private:
public:
	using _MyBase = instance<process_indicator, true>;
	instance(const string_indicator::tag& commandline = make_string("")) :_MyBase(), __init(commandline) {}
	explicit instance(ffmpeg_indicator) :instance() {}
	instance(const instance& other) :instance(other.commandline) {}
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

#endif // !__FILE_CONVENTION_FFMPEG_INSTANCE
