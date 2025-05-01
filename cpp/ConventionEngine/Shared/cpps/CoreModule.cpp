#include "CoreModule.h"

using namespace ConventionEngine;

_Notnull_ constexpr const char* CEObject::GetName() const
{
	return this->m_name;
}

void CEObject::SetName(_In_ const char* name)
{
	string_indicator::c_strcpy_s(this->m_name, name, name_max_length);
}

#pragma region Memory

class RawT :public any_class
{
public:
	void* RawMemoryBuffer = nullptr;
	size_t RawMemoryBufferSize = 0;

	std::map<CEHandle, CEObject*> RawObjectHandles = {};
	std::map<CEObject*, CEHandle> RawRObjectHandles = {};
};

RawT Raw;

#pragma endregion

void InitConventionEngine(size_t memory_size)
{
	Raw.RawMemoryBufferSize = memory_size;
	Raw.RawMemoryBuffer = ::malloc(sizeof(char) * Raw.RawMemoryBufferSize);
}
void ClearConventionEngine()
{
	memset(Raw.RawMemoryBuffer, 0, Raw.RawMemoryBufferSize);
}

CEHandle GetCEHandle(_In_opt_ CEObject* ptr)
{
	if (ptr == nullptr)
		return -1;
	if (Raw.RawRObjectHandles.count(ptr))
		return Raw.RawRObjectHandles[ptr];
	return -1;
}
_Ret_maybenull_ CEObject* GetCEObject(CEHandle handle)
{
	if (Raw.RawObjectHandles.count(handle))
		return Raw.RawObjectHandles[handle];
	return nullptr;
}

_Ret_notnull_ const char* GetName(CEHandle handle)
{
	return GetCEObject(handle)->GetName();
}

void SetName(CEHandle handle, _In_ const char* name)
{
	GetCEObject(handle)->SetName(name);
}