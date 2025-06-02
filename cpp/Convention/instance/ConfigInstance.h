#ifndef __FILE_CONVENTION_CONFIG_INSTANCE
#define __FILE_CONVENTION_CONFIG_INSTANCE
#include "Convention/instance/Interface.h"

namespace Convention
{

	template<template<typename...> class Allocator>
	class instance<CommandLineReader, true, Allocator, true>
		: public instance<CommandLineReader, false, Allocator, true>
	{
	private:
		using _Mybase = instance<CommandLineReader, false, Allocator, true>;
	public:
		instance(int argc, char** argv) :_Mybase(BuildMyPtr(argc, argv)) {}
		instance(const instance& other) noexcept :_Mybase(other) {}
		instance(instance&& other) noexcept : _Mybase(std::move(other)) {}
		instance& operator=(const instance& data) noexcept
		{
			_Mybase::operator=(data);
			return *this;
		}
		instance& operator=(instance&& data) noexcept
		{
			_Mybase::operator=(std::move(data));
			return *this;
		}
		virtual ~instance() {}

		const decltype(std::declval<CommandLineReader>().KeyValuePair)& ReadDictionary() const
		{
			return this->ReadConstValue().KeyValuePair;
		}
		const decltype(std::declval<CommandLineReader>().KeyVector)& ReadVector() const
		{
			return this->ReadConstValue().KeyVector;
		}

		std::map<std::string, std::string> try_get_histroy;

		std::filesystem::path ReadExecutePath() const
		{
			auto iter = this->ReadDictionary().find("execute");
			if (iter != this->ReadDictionary().end())
				return iter->second;
			throw std::filesystem::filesystem_error(
				"commandline is not setting",
				std::make_error_code(std::errc::not_supported)
			);
		}
		auto Contains(const std::string& key) const
		{
			return this->ReadDictionary().count(key);
		}
		auto GetValue(const std::string& key) const -> decltype(std::declval<instance>().ReadDictionary().find(key)->second)
		{
			return this->ReadDictionary().find(key)->second;
		}
		template<typename Ret>
		Ret GetValueAs(const std::string& key) const
		{
			return StringIndicator::ToValue<Ret>(this->GetValue(key));
		}
		std::vector<std::string> GetValueVector(const std::string& key) const
		{
			auto&& vec = this->ReadVector();
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
		bool TryGetValue(const std::string& key, _Inout_ _Val* output) const
		{
			if (Contains(key))
			{
				*output = StringIndicator::ToValue<_Val>(this->GetValue(key));
				return true;
			}
			return false;
		}

		bool IsContainsHelperCommand() const
		{
			return this->Contains("h") ||
				this->Contains("help") ||
				this->Contains("?") ||
				this->Contains("manual") ||
				this->Contains("/?");
		}
		bool IsContainsVersionCommand() const
		{
			return this->Contains("v") || this->Contains("version");
		}
		std::string version() const
		{
			static std::string result;
			if (false == result.empty())
				return result;
			result.reserve(1024);
			result += StringIndicator::Combine<decltype(result)>("build in platform: ", PlatformIndicator::PlatformInfomation, "\n");
			result += StringIndicator::Combine<decltype(result)>("where: <", __LINE__, "> at ", __FILE__, "\n");
			result += StringIndicator::Combine<decltype(result)>("when: ", __DATE__, " ", __TIME__, "\n");
#ifdef CURRENT_COM_NAME
			result += Combine("who: ", CURRENT_COM_NAME, "\n");
#endif // CURRENT_COM_NAME
#ifdef CURRENT_APP_NAME
			result += Combine("app: ", CURRENT_APP_NAME, "\n");
#endif // CURRENT_APP_NAME
			return result;
		}
	private:
		std::string InternalMakeManualSummary(bool is_necessary, const std::string& key) const
		{
			if (is_necessary || key.front() == '[')
				return key;
			else
			{
				return StringIndicator::Combine<std::string>("[", key, "]");
			}
		}
		template<typename... _Args>
		std::string InternalMakeManualSummary(bool is_necessary, const std::string& key, const _Args&... args) const
		{
			return InternalMakeManualSummary(is_necessary, key) + " " + InternalMakeManualSummary(args...);
		}
	public:
		template<typename _Val>
		std::string MakeManual(const DescriptiveIndicator<_Val>& key)
		{
			return StringIndicator::Combine<std::string>("\t\t[", key.target, "] \t", key.description, "\n");
		}
		std::string MakeManual(const DescriptiveIndicator<void>& layer)
		{
			return StringIndicator::Combine<std::string>("\t", layer.description, ":\n");
		}
		std::string MakeManual(const std::string& top)
		{
			return top + "\n";
		}
		template<typename _First, typename _Second>
		std::string MakeManual(_First&& key, _Second&& second)
		{
			return MakeManual(std::forward<_First>(key)) + MakeManual(std::forward<_Second>(second));
		}
		template<typename _First, typename _Second, typename... _Args>
		std::string MakeManual(_First&& key, _Second&& second, _Args&&... args)
		{
			return MakeManual(std::forward<_First>(key)) + MakeManual(std::forward<_Second>(second)) + MakeManual(std::forward<_Args>(args)...);
		}
		template<typename... _Args>
		std::string MakeManualSummary(bool is_necessary, const std::string& key, const _Args&... args) const
		{
			return this->GetExecutePath().string() + " " + InternalMakeManualSummary(is_necessary, key, args...);
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
				char buffer[1024] = { 0 };
				snprintf(buffer, 1024, not_found_message_format.c_str(), find_key.c_str());
				std::cerr << "\n";
				throw std::bad_exception();
			}
			else
			{
				char buffer[1024] = { 0 };
				snprintf(buffer, 1024, not_found_message_format.c_str(), find_key.c_str());
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
			if (this->Contains(find_key))
			{
				target = StringIndicator::ToValue<_Val>(this->GetValue(find_key));
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

}

#endif // !__FILE_CONVENTION_CONFIG_INSTANCE
