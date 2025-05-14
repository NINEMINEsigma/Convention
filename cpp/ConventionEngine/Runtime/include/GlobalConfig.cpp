#include "ConventionEngine/Runtime/GlobalConfig.h"

using namespace ConventionEngine;

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
)
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

}

tool_file GlobalConfig::GetConfigFile()
{
	return data_dir | my_const_config_file;
}

tool_file GlobalConfig::GetFile(const std::string& file, bool isMustExist = false) const
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
	auto config = GetConfigFile();
	instance<json_indicator<std::map<std::string, dataTypePair>, true>, true> jsondata(R"(
{
	"properties":{}
}
)", true);
	//TODO
	config.data = (nlohmann::json)*jsondata;
}
GlobalConfig& GlobalConfig::LoadProperties()
{
}