#include "ConventionEngine/Runtime/GlobalConfig.h"

using namespace ConventionEngine;

std::map<const GlobalConfig* const, tool_file> logger_files;

void GlobalConfig::GenerateEmptyConfigJson(tool_file file)
{
	file.open(std::ios::out);
	nlohmann::json jsondata = R"({
	"properties":{}
})";
	file.data = jsondata;
	file.Save<nlohmann::json>();
}

GlobalConfig::GlobalConfig(
	bool isTryCreateDataDir,
	bool isLoad
) :GlobalConfig(tool_file("./"), isTryCreateDataDir, isLoad)
{

}
GlobalConfig::GlobalConfig(
	tool_file dataDir,
	bool isTryCreateDataDir,
	bool isLoad
) : data_dir(".")
{
	// build up data folder
	if (dataDir.is_dir() == false)
		dataDir.back_to_parent_dir();
	if (dataDir.exist() == false)
	{
		if (isTryCreateDataDir)
			dataDir.must_exist_path();
		else
			throw std::runtime_error(Combine("Can't find data dir: ", *dataDir));
	}
	this->data_dir = std::move(dataDir);
	this->my_const_config_file = const_config_file;
	// build up init data file
	auto config_file = GetConfigFile();
	if (config_file.exist() == false)
	{
		GenerateEmptyConfigJson(config_file);
	}
	else
	{
		LoadProperties();
	}
}
GlobalConfig::~GlobalConfig()
{
	logger_files.erase(this);
}

tool_file GlobalConfig::GetConfigFile() const
{
	return data_dir | my_const_config_file;
}

tool_file GlobalConfig::GetFile(const std::string& file, bool isMustExist) const
{
	auto result = data_dir | file;
	if (isMustExist)
		result.must_exist_path();
	return result;
}
bool GlobalConfig::EraseFile(const std::string& file) const
{
	auto result = data_dir | file;
	if (result.exist())
	{
		result.open(std::ios::out);
		return true;
	}
	return false;
}
bool GlobalConfig::RemoveFile(const std::string& file) const
{
	auto result = data_dir | file;
	if (result.exist())
	{
		result.remove();
		return true;
	}
	return false;
}
bool GlobalConfig::CreateFileWhenNotExist(const std::string& file) const
{
	auto result = data_dir | file;
	if (result.exist())
	{
		return false;
	}
	if (result.get_parent_dir().exist() == false)
	{
		return false;
	}
	result.create();
	return true;
}

GlobalConfig::dataTypePairValueType& GlobalConfig::operator[](const dataTypePairKeyType& key)
{
	return data_pair[key];
}
bool GlobalConfig::Contains(const dataTypePairKeyType& key) const
{
	return !!data_pair.count(key);
}
bool GlobalConfig::RemoveItem(const dataTypePairKeyType& key)
{
	if (Contains(key))
	{
		data_pair.erase(key);
		return true;
	}
	return false;
}
auto GlobalConfig::begin() ->decltype(data_pair.begin())
{
	return data_pair.begin();
}
auto GlobalConfig::end() -> decltype(data_pair.end())
{
	return data_pair.end();
}

GlobalConfig& GlobalConfig::SaveProperties()
{
	using _json_indicator = json_indicator < std::map<std::string, dataTypePair>, true>;
	using _json_instance = instance<_json_indicator, true>;

	auto config = GetConfigFile();
	_json_instance jsondata(R"(
{
	"properties":{}
}
)", true);
	_json_indicator jsonindicator;
	for (auto&& [key, value] : data_pair)
	{
		jsondata["properties"][key] = value;
	}
	config.data = (nlohmann::json)*jsondata;
	config.Save();
	return *this;
}
GlobalConfig& GlobalConfig::LoadProperties()
{
	using _json_indicator = json_indicator < std::map<std::string, dataTypePair>, true>;
	using _json_instance = instance<_json_indicator, true>;
	auto config = GetConfigFile();
	_json_instance jsondata(config);
	_json_indicator jsonindicator;
	for (auto&& [key, value] : jsondata["properties"].items())
	{
		data_pair[key] = value;
	}
	return *this;
}

tool_file GlobalConfig::GetLogFile() const
{
	if(logger_files.count(this))
		return logger_files[this];
	auto log_file = GetFile(
		std::to_string(GetConfigFile().get_filename(true)) + "_log.txt",
		true);
	log_file.open(std::ios::app | std::ios::out);
	logger_files[this] = log_file;
	return log_file;
}
void GlobalConfig::Log(
	const std::string& messageType,
	const std::string& message,
	ConsoleColor color
) const
{
	auto log = GetLogFile();
	auto what = messageType + ": " + message;
	log.get_stream<std::ofstream>() << color << what 
		<< ConsoleColor::None << std::endl;
}
void GlobalConfig::Log(
	const std::string& messageType,
	const std::string& message,
	ConsoleBackgroundColor color
) const
{
	auto log = GetLogFile();
	auto what = messageType + ": " + message;
	log.get_stream<std::ofstream>() << color << what 
		<< ConsoleBackgroundColor::None << std::endl;
}
void GlobalConfig::LogMessage(
	const std::string& message,
	ConsoleColor color
) const
{
	Log("Message", message, color);
}
void GlobalConfig::LogMessage(
	const std::string& message,
	ConsoleBackgroundColor color
) const
{
	Log("Message", message, color);
}
void GlobalConfig::LogMessage(
	const std::string& message
) const
{
	LogMessage(message, ConsoleColor::Blue);
}
void GlobalConfig::LogWarning(const std::string& message) const
{
	Log("Warning", message, ConsoleColor::Yellow);
}
void GlobalConfig::LogError(const std::string& message) const
{
	Log("Error", message, ConsoleColor::Red);
}
void GlobalConfig::LogPropertyNotFound(const std::string& key) const
{
	Log("PropertyNotFound", key, ConsoleColor::Yellow);
}
void GlobalConfig::LogPropertyNotFound(
	const std::string& key,
	const std::string& defaultMessage
) const
{
	Log("PropertyNotFound", key + "(default=" + defaultMessage + ")", ConsoleColor::Yellow);
}
void GlobalConfig::LogMessageOfPleaseCompleteConfiguration() const
{
	Log("PleaseCompleteConfiguration", "Please complete configuration", ConsoleColor::Red);
}

const GlobalConfig::dataTypePairValueType& GlobalConfig::FindItem(
	const dataTypePairKeyType& key
) const
{
	if (Contains(key))
	{
		return data_pair.at(key);
	}
	else
	{
		LogPropertyNotFound(key);
		LogMessageOfPleaseCompleteConfiguration();
		throw std::runtime_error("Property not found: " + key);
	}
}
const GlobalConfig::dataTypePairValueType& GlobalConfig::FindItem(
	const dataTypePairKeyType& key,
	const dataTypePairValueType& defaultValue
) const
{
	if (Contains(key))
	{
		return data_pair.at(key);
	}
	else
	{
		LogPropertyNotFound(key);
		return defaultValue;
	}
}

tool_file ProjectConfig::ProjectConfigFileFocus = "./Assets/";
ProjectConfig::ProjectConfig(
	bool isLoad
) : GlobalConfig(ProjectConfigFileFocus, true, isLoad)
{
}
ProjectConfig::~ProjectConfig()
{
}

void ProjectConfig::SetProjectAssets(const std::string& path)
{
	ProjectConfigFileFocus.open(path);
}
std::filesystem::path ProjectConfig::GetProjectAssets()
{
	return *ProjectConfigFileFocus;
}
