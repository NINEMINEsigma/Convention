#ifndef __FILE_CONVENTION_FFMPEG_INSTANCE
#define __FILE_CONVENTION_FFMPEG_INSTANCE

#include "Convention/Interface.h"
#include "Convention/process_instance.h"
#include <vector>
#include <map>

struct ffmpeg_indicator
{
	using tag = string_indicator::tag;
	static constexpr bool value = true;
};

// ffmpeg 配置选项结构体
struct ffmpeg_config 
{
	// 视频相关
	int width = -1;                  		// 视频宽度
	int height = -1;                 		// 视频高度
	std::string video_codec;         		// 视频编码器
	int video_bitrate = -1;          		// 视频比特率
	float fps = -1;                  		// 帧率

	// 音频相关		
	std::string audio_codec;         		// 音频编码器
	int audio_bitrate = -1;          		// 音频比特率
	int sample_rate = -1;            		// 采样率
	int audio_channels = -1;         		// 音频通道数

	// 通用选项		
	bool overwrite = false;          		// 是否覆盖输出文件
	std::string preset;              		// 编码预设
	std::string format;              		// 输出格式
	std::vector<std::string> extra_options; // 额外选项
};

template<>
class instance<ffmpeg_indicator, true> :public instance<process_indicator, true>
{
private:
	using _Mybase = instance<process_indicator, true>;
	ffmpeg_config config;
	
	// 将配置转换为命令行参数
	string_indicator::tag build_options() const 
	{
		string_indicator::tag options;
		
		if (config.overwrite) 
		{
			options += make_string(" -y");
		}
		
		if (config.width > 0 && config.height > 0) 
		{
			options += make_string(" -s ") + string_indicator::to_string(config.width) + make_string("x") + string_indicator::to_string(config.height);
		}
		
		if (!config.video_codec.empty())
		{
			options += make_string(" -c:v ") + string_indicator::to_string(config.video_codec);
		}
		
		if (config.video_bitrate > 0) 
		{
			options += make_string(" -b:v ") + string_indicator::to_string(config.video_bitrate) + make_string("k");
		}
		
		if (config.fps > 0) 
		{
			options += make_string(" -r ") + string_indicator::to_string(config.fps);
		}
		
		if (!config.audio_codec.empty())
		{
			options += make_string(" -c:a ") + string_indicator::to_string(config.audio_codec);
		}
		
		if (config.audio_bitrate > 0)
		 {
			options += make_string(" -b:a ") + string_indicator::to_string(config.audio_bitrate) + make_string("k");
		}
		
		if (config.sample_rate > 0) 
		{
			options += make_string(" -ar ") + string_indicator::to_string(config.sample_rate);
		}
		
		if (config.audio_channels > 0)
		{
			options += make_string(" -ac ") + string_indicator::to_string(config.audio_channels);
		}
		
		if (!config.preset.empty()) 
		{
			options += make_string(" -preset ") + string_indicator::to_string(config.preset);
		}
		
		if (!config.format.empty()) 
		{
			options += make_string(" -f ") + string_indicator::to_string(config.format);
		}
		
		for (const auto& opt : config.extra_options) 
		{
			options += make_string(" ") + string_indicator::to_string(opt);
		}
		
		return options;
	}

public:
	string_indicator::tag commandline;

public:
	instance(const string_indicator::tag& commandline = make_string("")) 
		:_Mybase(), commandline(commandline) {}
	explicit instance(ffmpeg_indicator) :instance() {}
	instance_move_operator(public)
	 {
		this->commandline = std::move(other.commandline);
		this->config = std::move(other.config);
	}
	virtual ~instance() {}

	// 基础操作
	bool ffplay(const string_indicator::tag& target) 
	{
		return this->exc((
			make_string("ffplay ") + this->commandline + 
			make_string(" \"") + target + make_string("\"")
		).c_str());
	}

	bool ffmpeg(const string_indicator::tag& input, const string_indicator::tag& output) 
	{
		auto options = build_options();
		return this->exc((
			make_string("ffmpeg ") + this->commandline + options +
			make_string(" -i \"") + input + make_string("\"") +
			make_string(" \"") + output + make_string("\"")
		).c_str());
	}

	// 配置设置器
	void set_video_size(int width, int height)
	 {
		config.width = width;
		config.height = height;
	}

	void set_video_codec(const std::string& codec)
	 {
		config.video_codec = codec;
	}

	void set_video_bitrate(int bitrate) 
	{
		config.video_bitrate = bitrate;
	}

	void set_fps(float fps) 
	{
		config.fps = fps;
	}

	void set_audio_codec(const std::string& codec)
	 {
		config.audio_codec = codec;
	}

	void set_audio_bitrate(int bitrate)
	 {
		config.audio_bitrate = bitrate;
	}

	void set_sample_rate(int rate) 
	{
		config.sample_rate = rate;
	}

	void set_audio_channels(int channels)
	 {
		config.audio_channels = channels;
	}

	void set_overwrite(bool overwrite) 
	{
		config.overwrite = overwrite;
	}

	void set_preset(const std::string& preset)
	 {
		config.preset = preset;
	}

	void set_format(const std::string& format)
	 {
		config.format = format;
	}

	void add_extra_option(const std::string& option) 
	{
		config.extra_options.push_back(option);
	}

	void clear_extra_options() {
		config.extra_options.clear();
	}

	// 高级功能
	bool extract_audio(const string_indicator::tag& input, const string_indicator::tag& output)
	{
		return this->exc((
			make_string("ffmpeg ") + this->commandline +
			make_string(" -i \"") + input + make_string("\"") +
			make_string(" -vn -acodec copy \"") + output + make_string("\"")
		).c_str());
	}

	bool extract_video(const string_indicator::tag& input, const string_indicator::tag& output) 
	{
		return this->exc((
			make_string("ffmpeg ") + this->commandline +
			make_string(" -i \"") + input + make_string("\"") +
			make_string(" -an -vcodec copy \"") + output + make_string("\"")
		).c_str());
	}

	bool cut_video(
		const string_indicator::tag& input, 
		const string_indicator::tag& output,
		const string_indicator::tag& start_time,
		const string_indicator::tag& duration
		)
	{
		return this->exc((
			make_string("ffmpeg ") + this->commandline +
			make_string(" -i \"") + input + make_string("\"") +
			make_string(" -ss ") + start_time +
			make_string(" -t ") + duration +
			make_string(" -c copy \"") + output + make_string("\"")
		).c_str());
	}

	bool merge_videos(const std::vector<string_indicator::tag>& inputs, const string_indicator::tag& output) 
	{
		string_indicator::tag input_list;
		for (const auto& input : inputs) 
		{
			input_list += make_string(" -i \"") + input + make_string("\"");
		}
		
		return this->exc((
			make_string("ffmpeg ") + this->commandline +
			input_list +
			make_string(" -filter_complex concat=n=") + string_indicator::to_string(inputs.size()) + make_string(":v=1:a=1 \"") +
			output + make_string("\"")
		).c_str());
	}

	bool create_thumbnail(
		const string_indicator::tag& input, const string_indicator::tag& output,
		const string_indicator::tag& time = make_string("00:00:01")
		) 
	{
		return this->exc((
			make_string("ffmpeg ") + this->commandline +
			make_string(" -i \"") + input + make_string("\"") +
			make_string(" -ss ") + time +
			make_string(" -vframes 1 \"") + output + make_string("\"")
		).c_str());
	}

	bool add_watermark(
		const string_indicator::tag& input, 
		const string_indicator::tag& watermark,
		const string_indicator::tag& output, 
		const string_indicator::tag& position = make_string("overlay=10:10")
		) 
	{
		return this->exc((
			make_string("ffmpeg ") + this->commandline +
			make_string(" -i \"") + input + make_string("\"") +
			make_string(" -i \"") + watermark + make_string("\"") +
			make_string(" -filter_complex \"") + position + make_string("\" \"") +
			output + make_string("\"")
		).c_str());
	}

	bool convert_format(
		const string_indicator::tag& input, 
		const string_indicator::tag& output,
		const std::string& format
		) 
	{
		set_format(format);
		return ffmpeg(input, output);
	}

	// 获取媒体信息
	bool probe(const string_indicator::tag& input)
	{
		return this->exc((
			make_string("ffprobe -v quiet -print_format json -show_format -show_streams \"") +
			input + make_string("\"")
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
