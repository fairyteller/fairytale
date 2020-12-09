#pragma once
#include "fairy_object.h"

class Runtime;

class ObjectRef
{
public:
	static const objectId INVALID_ID = -1;

	ObjectRef(Runtime* pRuntime, objectId id = INVALID_ID);
	ObjectRef(const ObjectRef& rhs);
	const ObjectRef& operator=(const ObjectRef& rhs);
	ObjectRef(ObjectRef&& rhs);
	const ObjectRef& operator=(ObjectRef&& rhs);
	~ObjectRef();

	FairyObject* operator->() const;
	objectId id()  const { return m_id; }
	void reset(objectId id);
private:
	objectId m_id;
	Runtime* pRuntime;
};

template<class T>
class alloc_guard
{
public:
	alloc_guard(Runtime* pRuntime, T* ptr) : pData(ptr), pRuntime(pRuntime)
	{
		pRuntime->direct_memory_usage_semaphore++;
	};
	alloc_guard(const alloc_guard<T>& rhs) : pData(rhs.pData), pRuntime(rhs)
	{
		pRuntime->direct_memory_usage_semaphore++;
	}
	//const alloc_guard<T>& operator=(const alloc_guard<T>& rhs)
	~alloc_guard()
	{
		pRuntime->direct_memory_usage_semaphore++;
	}

	T* operator->() const { return pData; }
private:
	T* pData;
	Runtime* pRuntime;
};