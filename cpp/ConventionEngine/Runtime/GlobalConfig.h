#ifndef __FILE_CE_GLOBALCONFIG
#define __FILE_CE_GLOBALCONFIG

#include "Convention/Instance.h"

#define CE ConventionEngine::

namespace ConventionEngine
{
	class GlobalConfig :public any_class
	{
	public:
		using dataTypePairKeyType = std::string;
		using dataTypePairValueType = std::any;
		using dataTypePair = std::map<dataTypePairKeyType, dataTypePairValueType>;
	private:
		tool_file data_dir;
		dataTypePair data_pair;
		std::string my_const_config_file;
	public:
		int configLogging_tspace = 0;
		static std::string const_config_file;

		static void GenerateEmptyConfigJson(tool_file file);

		GlobalConfig(
			bool isTryCreateDataDir = false,
			bool isLoad = true
		);
		GlobalConfig(
			tool_file dataDir,
			bool isTryCreateDataDir = false,
			bool isLoad = true
		);
		~GlobalConfig();

		tool_file GetConfigFile();

		tool_file GetFile(const std::string& file, bool isMustExist = false) const;
		bool EraseFile(const std::string& file) const;
		bool RemoveFile(const std::string& file) const;
		bool CreateFileWhenNotExist(const std::string& file) const;

		dataTypePairValueType& operator[](const dataTypePairKeyType& key);
		bool Contains(const dataTypePairKeyType& key) const;
		bool RemoveItem(const dataTypePairKeyType& key);
		size_t DataPairSize() const;
		typename dataTypePair::iterator begin();
		typename dataTypePair::iterator end();

		GlobalConfig& SaveProperties();
		GlobalConfig& LoadProperties();

		tool_file GetLogFile() const;
		virtual void Log(
			const std::string& messageType,
			const std::string& message,
			ConsoleColor color
		) const;
		virtual void Log(
			const std::string& messageType,
			const std::string& message,
			ConsoleBackgroundColor color
		) const;
		GlobalConfig& LogMessage(
			const std::string& message,
			ConsoleColor color
		) const;
		GlobalConfig& LogMessage(
			const std::string& message,
			ConsoleBackgroundColor color
		) const;
		GlobalConfig& LogMessage(
			const std::string& message
		) const;
		GlobalConfig& LogWarning(const std::string& message) const;
		GlobalConfig& LogError(const std::string& message) const;
		GlobalConfig& LogPropertyNotFound(const std::string& key) const;
		GlobalConfig& LogPropertyNotFound(
			const std::string& key,
			const std::string& defaultMessage
		) const;
		GlobalConfig& LogMessageOfPleaseCompleteConfiguration() const;

		GlobalConfig& LogClear() const;

		dataTypePairValueType& FindItem(
			const dataTypePairKeyType& key,
			const dataTypePairValueType& defaultValue
		) const;
	};

	class ProjectConfig : public GlobalConfig
	{
	public:
		static std::string ProjectConfigFileFocus;
		ProjectConfig(bool isLoad = true);
		~ProjectConfig();

		static void SetProjectAssets(const std::string& path);
		static std::string GetProjectAssets();
	};
}

#endif // !__FILE_CE_GLOBALCONFIG
