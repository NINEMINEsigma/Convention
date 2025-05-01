#include "CoreModule.h"

using namespace ConventionEngine;

CEObject::~CEObject()
{

}

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
private:
	CEHandle handle_tail = 0;

	std::map<CEHandle, CEObject*> RawObjectHandles = {};
	std::map<CEObject*, CEHandle> RawRObjectHandles = {};

	void* RawMemoryBuffer = nullptr;
	size_t RawMemoryBufferSize = 0;

	std::multimap<size_t, void*> FreeBlockContainer = {};
public:

	void Insert(_In_ CEObject* ptr)
	{
		RawObjectHandles[handle_tail] = ptr;
		RawRObjectHandles[ptr] = handle_tail;
		++handle_tail;
	}
	_Ret_maybenull_ CEObject* At(CEHandle handle)
	{
		if (handle < 0 && handle >= handle_tail)
			return nullptr;
		return RawObjectHandles[handle];
	}
	CEHandle At(_In_opt_ CEObject* ptr)
	{
		if (ptr == nullptr)
			return -1;
		return RawRObjectHandles[ptr];
	}
	bool Contains(CEHandle handle)
	{
		return RawObjectHandles.count(handle);
	}
	bool Contains(_In_opt_ CEObject* ptr)
	{
		if (ptr == nullptr)
			return nullptr;
		return RawRObjectHandles.count(ptr);
	}
	friend void InitConventionEngine(size_t memory_size);
	friend void ClearConventionEngine();
	friend void QuitConventionEngine();
	friend void GC_ConventionEngine();

	_Ret_notnull_ void* MemoryAlloc(size_t size)
	{

	}
	void FreeMemory(_In_ void* ptr)
	{

	}
};

RawT Raw;

void InitConventionEngine(size_t memory_size)
{
	Raw.RawMemoryBufferSize = memory_size;
	Raw.RawMemoryBuffer = ::malloc(sizeof(char) * Raw.RawMemoryBufferSize);
	Raw.FreeBlockContainer.insert({ Raw.RawMemoryBufferSize, Raw.RawMemoryBuffer });
}
void ClearConventionEngine()
{
	memset(Raw.RawMemoryBuffer, 0, Raw.RawMemoryBufferSize);
	for (auto&& [_, ptr] : Raw.RawObjectHandles)
	{
		delete ptr;
	}
	Raw.RawObjectHandles.clear();
	Raw.RawRObjectHandles.clear();
	Raw.FreeBlockContainer.clear();
	Raw.FreeBlockContainer.insert({ Raw.RawMemoryBufferSize, Raw.RawMemoryBuffer });
}

void QuitConventionEngine()
{
	Raw.RawMemoryBufferSize = 0;
	Raw.FreeBlockContainer.clear();
	::free(Raw.RawMemoryBuffer);
}

void GC_ConventionEngine()
{

}

#pragma endregion

CEHandle GetCEHandle(_In_opt_ CEObject* ptr)
{
	if (ptr == nullptr)
		return -1;
	if (Raw.Contains(ptr))
		return Raw.At(ptr);
	return -1;
}
_Ret_maybenull_ CEObject* GetCEObject(CEHandle handle)
{
	if (Raw.Contains(handle))
		return Raw.At(handle);
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



void* CEObject::operator new(size_t size)
{
	return Raw.MemoryAlloc(size);
}