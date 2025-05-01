#ifndef __FILE_CONVENTION_ENGINE_INTERNAL

#define __FILE_CONVENTION_ENGINE_INTERNAL

#include "Config.h"
#include "Instance.h"

namespace ConventionEngine
{
	class CEObject
	{
	public:
		constexpr static size_t name_max_length = __NameMaxLength;
	private:
		int m_instanceID = -1;
		char m_name[name_max_length] = { 0 };

	public:

		constexpr const char* GetName() const;
		void SetName(const char* name);
	};

	using CEHandle = int_fast64_t;
}

#define CE ConventionEngine::

extern "C"
{
	void InitConventionEngine(size_t memory_size);
	void ClearConventionEngine();
	void QuitConventionEngine();
	CE CEHandle GetCEHandle(CE CEObject* ptr);
	CE CEObject* GetCEObject(CE CEHandle handle);

	const char* GetName(CE CEHandle handle);
	void SetName(CE CEHandle handle, const char* name);
}

#endif // !__FILE_CONVENTION_ENGINE_INTERNAL
