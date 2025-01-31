#ifndef __FILE_CONVENTION_CONFIG_INSTANCE
#define __FILE_CONVENTION_CONFIG_INSTANCE

#include "Convention/Interface.h"

struct config_indicator
{
	using tag = std::decay_t<decltype(make_config(0, nullptr))>;
	constexpr static bool value = true;
};
template<>
class instance<config_indicator::tag, true> :public instance<config_indicator::tag, false>
{
private:
	using _Mybase = instance<config_indicator::tag, false>;
public:
	explicit instance(int argc, char** argv) :_Mybase(
		new config_indicator::tag(make_config(argc, argv))
	) {}
	instance(const _shared& data) noexcept :_Mybase(data) {}
	instance& operator=(const _shared& data) noexcept
	{
		_Mybase::operator=(data);
	}
	virtual ~instance() {}

	constexpr const auto& dict() const
	{
		return std::get<0>(**this);
	}
	constexpr const auto& vec() const
	{
		return std::get<1>(**this);
	}

	std::filesystem::path execute_path() const
	{
		auto iter = this->dict().find("execute");
		if (iter != this->dict().end())
			return iter->second;
		throw std::filesystem::filesystem_error(
			"commandline is not setting",
			std::make_error_code(std::errc::not_supported)
		);
	}
	auto contains(const std::string& key) const
	{
		return this->dict().count(key);
	}
	template<typename _Val>
	_Val value(const std::string& key) const
	{
		return convert_xvalue<_Val>(this->dict().find(key)->second);
	}
	int int_value(const std::string& key) const
	{
		return value<int>(key);
	}
	double float_value(const std::string& key) const
	{
		return value<double>(key);
	}
	std::string string_value(const std::string& key) const
	{
		return value<std::string>(key);
	}
	bool bool_value(const std::string& key) const
	{
		return value<bool>(key);
	}
	std::vector<std::string> list(const std::string& key) const
	{
		auto&& vec = this->vec();
		std::vector<std::string> result;

		auto iter = std::find_if(vec.begin(), vec.end(), [&key](const auto& pair)
			{
				std::string str = pair.first;
				while (str.size() && str.front() == '-')
					str.erase(str.begin());
				return str == key;
			});
		if (iter == vec.end())
			return {};
		if (iter->second.size() != 0)
			result.push_back(iter->second);
		while (++iter != vec.end())
		{
			if (iter->first.front() == '-')
				break;
			result.push_back(iter->first);
		}
		return result;
	}

	template<typename _Val>
	_Val try_value(const std::string& key, _Val default_val) const
	{
		auto iter = this->dict().find(key);
		if (iter != this->dict().end())
			return convert_xvalue<_Val>(iter->second);
		return default_val;
	}
	int try_int_value(const std::string& key, int default_val) const
	{
		return try_value<int>(key, default_val);
	}
	double try_float_value(const std::string& key, double default_val) const
	{
		return try_value<double>(key, default_val);
	}
	std::string try_string_value(const std::string& key, const std::string& default_val) const
	{
		return try_value<std::string>(key, default_val);
	}
	bool try_bool_value(const std::string& key, bool default_val) const
	{
		return try_value<bool>(key, default_val);
	}

	bool is_contains_helper_command() const
	{
		return this->contains("h") || this->contains("help") || this->contains("?");
	}
	bool is_contains_version_command() const
	{
		return this->contains("v") || this->contains("version");
	}
	std::string version() const
	{
		std::string result;
		result.reserve(1024);
		result += Combine("build in platform: ", platform_indicator::generate_platform_message(), "\n");
		result += Combine("where: <", __LINE__, "> at ", __FILE__, "\n");
		result += Combine("when: ", __DATE__, " ", __TIME__, "\n");
#ifdef CURRENT_COM_NAME
		result += Combine("who: ", CURRENT_COM_NAME, "\n");
#endif // CURRENT_COM_NAME
#ifdef CURRENT_APP_NAME
		result += Combine("app: ", CURRENT_APP_NAME, "\n");
#endif // CURRENT_APP_NAME
		return result;
	}
private:
	template<typename _Val>
	std::string internal_make_manual_text(const descriptive_indicator<_Val>& key) const
	{
		return Combine("\t\t[", key.target, "] \t", key.description, "\n");
	}
	std::string internal_make_manual_text(const descriptive_indicator<void>& layer) const
	{
		return Combine("\t", layer.description, ":\n");
	}
	template<typename _First, typename... _Args>
	std::string inertnal_make_manual(const _First& key)
	{
		return internal_make_manual_text(key);
	}
	template<typename _First, typename... _Args>
	std::string inertnal_make_manual(const _First& key, const _Args&... args)
	{
		return internal_make_manual_text(key) + inertnal_make_manual(args...);
	}
	std::string internal_make_manual_summary(bool is_necessary, const std::string& key) const
	{
		if (is_necessary)
			return key;
		else
		{
			if (key.front() == '[')
				return key;
			else
				return Combine("[", key, "]");
		}
	}
	template<typename... _Args>
	std::string internal_make_manual_summary(bool is_necessary, const std::string& key, const _Args&... args) const
	{
		return internal_make_manual_summary(is_necessary, key) +" " + internal_make_manual_summary(args...);
	}
public:
	template<typename _First,typename... _Args>
	std::string make_manual(const std::string& top,const _First& key, const _Args&... args)
	{
		return top + "\n" + internal_make_manual_text(key) + inertnal_make_manual(args...);
	}
	template<typename... _Args>
	std::string make_manual_summary(bool is_necessary, const std::string& key, const _Args&... args) const
	{
		return this->execute_path().string() + " " + internal_make_manual_summary(is_necessary, key, args...);
	}

	template<typename _Val>
	bool operator()(
		const std::string& find_key,
		std::function<void(_Val)> action,
		bool necessary = false,
		const std::string& not_found_message_format = "the necessary argument has not given: %s is not found") const
	{
		if (this->contains(find_key))
		{
			action(convert_xvalue<_Val>(this->string_value(find_key)));
			return true;
		}
		else if (necessary)
		{
			char buffer[1024];
			sprintf(buffer, not_found_message_format.c_str(), find_key.c_str());
			std::cerr << "\n";
			throw std::bad_exception();
		}
		else
		{
			char buffer[1024];
			sprintf(buffer, not_found_message_format.c_str(), find_key.c_str());
			std::cout << "\n";
			return false;
		}
	}
	template<typename _Val>
	bool operator()(
		const std::string& find_key,
		_Val& target,
		bool necessary = false,
		const std::string& not_found_message_format = "the necessary argument has not given: %s is not found") const
	{
		if (this->contains(find_key))
		{
			target = convert_xvalue<_Val>(this->string_value(find_key));
			return true;
		}
		else if (necessary)
		{
			char buffer[1024];
			sprintf(buffer, not_found_message_format.c_str(), find_key.c_str());
			std::cerr << "\n";
			throw std::bad_exception();
		}
		else
		{
			char buffer[1024];
			sprintf(buffer, not_found_message_format.c_str(), find_key.c_str());
			std::cout << "\n";
			return false;
		}
	}
};

#endif // !__FILE_CONVENTION_CONFIG_INSTANCE
