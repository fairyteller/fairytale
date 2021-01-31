#pragma once
#include "fairy_object.h"

class Runtime;

void increment_semaphore(Runtime* pRuntime);
void decrement_semaphore(Runtime* pRuntime);

// while we operate with raw pointer we should be sure that no allocation happens that can invalidate them
template<class T>
class alloc_guard
{
public:
	alloc_guard(Runtime* pRuntime, T* ptr) : pData(ptr), pRuntime(pRuntime)
	{
		increment_semaphore(pRuntime);
	};
	alloc_guard(const alloc_guard<T>& rhs) : pData(rhs.pData), pRuntime(rhs)
	{
		increment_semaphore(pRuntime);
	}
	//const alloc_guard<T>& operator=(const alloc_guard<T>& rhs)
	~alloc_guard()
	{
		decrement_semaphore(pRuntime);
	}

	T* operator->() const { return pData; }
	T& operator*() const { return *pData; }
private:
	T* pData;
	Runtime* pRuntime;
};

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
	FairyObject& operator*() const;
	objectId id()  const { return m_id; }
	void reset(objectId id);
	alloc_guard<FairyObject> getPtr();
private:
	objectId m_id;
	Runtime* pRuntime;
};
