#pragma once
#include "fairy_object.h"

class Runtime;

class ObjectRef
{
	static const objectId INVALID_ID = -1;

	ObjectRef(Runtime* pRuntime, objectId id = INVALID_ID);
	ObjectRef(const ObjectRef& rhs);
	const ObjectRef& operator=(const ObjectRef& rhs);
	ObjectRef(ObjectRef&& rhs);
	const ObjectRef& operator=(ObjectRef&& rhs);
	~ObjectRef();

	FairyObject& operator->();
	objectId id() { return m_id; }
	void reset(objectId id);
private:
	objectId m_id;
	Runtime* pRuntime;
};