#ifndef __FILE_CONVENTION_ENGINE_INTERNAL

#define __FILE_CONVENTION_ENGINE_INTERNAL

#include "Config.h"
#include "Instance.h"

class CEObject
{
public:
	using string = std::string;

private:
	intptr_t m_cachedPtr = 0;
	int m_instanceID = -1;
	string m_name;


public:
	string name() noexcept;
	void named(const string& name);
private:

};


#endif // !__FILE_CONVENTION_ENGINE_INTERNAL
