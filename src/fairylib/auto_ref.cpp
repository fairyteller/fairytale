#include "auto_ref.h"
#include "fairytale_core.h"
#include "abstract_syntax_tree.h"

ObjectRef::ObjectRef(Runtime* pRuntime, objectId id)
	: m_id(id)
	, pRuntime(pRuntime)
{
	if (id != INVALID_ID)
		pRuntime->inc_ref(id);
}


ObjectRef::ObjectRef(const ObjectRef& rhs)
	: m_id(rhs.m_id)
	, pRuntime(rhs.pRuntime)
{
	if (m_id != INVALID_ID)
		pRuntime->inc_ref(m_id);
}

const ObjectRef& ObjectRef::operator=(const ObjectRef& rhs)
{
	if (m_id != INVALID_ID)
		pRuntime->dec_ref(m_id);
	m_id = rhs.m_id;
	pRuntime = rhs.pRuntime;
	if (m_id != INVALID_ID)
		pRuntime->inc_ref(m_id);
	return *this;
}

ObjectRef::ObjectRef(ObjectRef&& rhs)
	: m_id(rhs.m_id)
	, pRuntime(rhs.pRuntime)
{
	rhs.m_id = INVALID_ID;
}
const ObjectRef& ObjectRef::operator=(ObjectRef&& rhs)
{
	if (m_id != INVALID_ID)
		pRuntime->dec_ref(m_id);
	m_id = rhs.m_id;
	rhs.m_id = INVALID_ID;
	pRuntime = rhs.pRuntime;
	return *this;
}

ObjectRef::~ObjectRef()
{
	if (m_id != INVALID_ID)
		pRuntime->dec_ref(m_id);
}

FairyObject* ObjectRef::operator->()  const
{
	return pRuntime->getObject(m_id);
}

FairyObject& ObjectRef::operator*()  const
{
	return *(pRuntime->getObject(m_id));
}

void ObjectRef::reset(objectId id)
{
	if (m_id != INVALID_ID)
		pRuntime->dec_ref(m_id);
	m_id = id;
	if (m_id != INVALID_ID)
		pRuntime->inc_ref(m_id);
}

alloc_guard<FairyObject> ObjectRef::getPtr()
{
	return alloc_guard<FairyObject>(pRuntime, pRuntime->getObject(m_id));
}
