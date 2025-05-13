#include "ConventionEngine/Runtime/GlobalConfig.h"

using namespace ConventionEngine;

void GlobalConfig::GenerateEmptyConfigJson(tool_file file)
{
	file.open(std::ios::out);
	//file.
}

GlobalConfig::GlobalConfig(
	bool isTryCreateDataDir = false,
	bool isLoad = true
) :GlobalConfig(tool_file("./"), isTryCreateDataDir, isLoad)
{

}
GlobalConfig::GlobalConfig(
	tool_file dataDir,
	bool isTryCreateDataDir = false,
	bool isLoad = true
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