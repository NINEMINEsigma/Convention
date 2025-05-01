#ifndef __FILE_CONVENTION_ENGINE_INTERNAL

#define __FILE_CONVENTION_ENGINE_INTERNAL

#include "Convention/Config.h"
#include "Convention/Instance.h"

namespace ConventionEngine
{
	class CEObject
	{
	public:
		constexpr static size_t name_max_length = __NameMaxLength;
	private:
		int m_instanceID = -1;
		char m_name[name_max_length] = { 0 };

	protected:
		void* operator new(size_t size);

	public:
		void operator delete(void* ptr, size_t size);
		virtual ~CEObject();

		_Notnull_ constexpr const char* GetName() const;
		void SetName(_In_ const char* name);
	};


	using CEError = size_t;
	using CEHandle = int_fast64_t;
}

#define CE ConventionEngine::

extern "C"
{
	void InitConventionEngine(size_t memory_size);
	void ClearConventionEngine();
	void QuitConventionEngine();

	void GC_ConventionEngine();

	CE CEHandle GetCEHandle(_In_opt_ CE CEObject* ptr);
	_Ret_maybenull_ CE CEObject* GetCEObject(CE CEHandle handle);

	_Ret_notnull_ const char* GetName(CE CEHandle handle);
	void SetName(CE CEHandle handle, _In_ const char* name);
}

#endif // !__FILE_CONVENTION_ENGINE_INTERNAL
