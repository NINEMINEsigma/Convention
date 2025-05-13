#ifndef __FILE_CE_GLOBALCONFIG
#define __FILE_CE_GLOBALCONFIG

#include "Convention/Instance.h"

#define CE ConventionEngine


namespace ConventionEngine
{
	class GlobalConfig :public any_class
	{
	public:
		int configLogging_tspace = 0;
		tool_file GetConfigFile()
		{

		}
	};
}

#endif // !__FILE_CE_GLOBALCONFIG
