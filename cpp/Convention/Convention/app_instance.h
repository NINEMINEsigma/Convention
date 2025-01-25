#ifndef __FILE_CONVENTION_APP_INSTANCE
#define __FILE_CONVENTION_APP_INSTANCE

#include "Convention/Interface.h"
#include "Convention/file_instance.h"

template<>
class instance<platform_indicator, true> :public any_class
{
public:
	instance(){}
	instance(const instance&) = delete;
	virtual ~instance() {}

	using string_value_type = const string_indicator::tag&;

	string_value_type platform() const noexcept
	{
		static auto str = make_string(__PLATFORM_NAME);
		return str;
	}
	string_value_type version() const noexcept
	{
		static auto str = make_string(__PLATFORM_VERSION);
		return str;
	}
	string_value_type extension() const noexcept
	{
		static auto str = make_string(__PLATFORM_EXTENSION);
		return str;
	}
	const auto& application_path() const noexcept
	{
		static auto path = std::filesystem::current_path();
		return path;
	}
	const auto& streaming_assets_path() const noexcept
	{
		static auto path = std::filesystem::current_path() / "StreamingAssets/";
		return path;
	}
	const std::filesystem::path& persistent_path() const noexcept;
	
};

#endif // !__FILE_CONVENTION_APP_INSTANCE
