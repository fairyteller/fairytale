#include "fairy_object.h"
#include "fairytale_core.h"

std::string FairyObject::describe()
{
	std::stringstream ss;
	if (m_type == FairyObjectType::Int)
		ss << "<Integer: " << asLong() << ">";
	else if (m_type == FairyObjectType::Bool)
		ss << "<Boolean: " << asBool() << ">";
	else if (m_type == FairyObjectType::String)
		ss << "<String: " << asString() << ">";
	else if (m_type == FairyObjectType::FairyFunction)
		ss << "<FairyFunction at " << reinterpret_cast<long long>(asFairyFunction()) << " >";
	else if (m_type == FairyObjectType::WrappedFunction)
		ss << "<WrappedFunction at " << reinterpret_cast<long long>(asWrappedFunction()) << " >";
	ss << "(";
	ss << refCount;
	ss << ")";
	return ss.str();
}


void FairyObject::setattr(Runtime* pRuntime, stringId strid, objectId objid)
{
	auto iter = table.find(strid);
	if (iter != table.end())
	{
		// there is an object referenced by this key
		pRuntime->dec_ref(iter->second);
	}
	pRuntime->inc_ref(objid);
	table[strid] = objid;
}

objectId FairyObject::getattr(Runtime* pRuntime, stringId key)
{
	auto iter = table.find(key);
	if (iter != table.end())
		return iter->second;

	//or there is no object referenced by this key, return empty
	return pRuntime->allocate(FairyObjectType::Unknown);
}


void FairyObject::assign(Runtime* pRuntime, FairyObject* rhs)
{
	m_type = rhs->m_type;
	m_value = rhs->m_value;
	table = rhs->table;
	for (auto& attrib : table)
	{
		pRuntime->inc_ref(attrib.second);
	}
}

void FairyObject::release(Runtime* pRuntime)
{
	for (auto& attrib : table)
	{
		pRuntime->dec_ref(attrib.second);
	}
	table.clear();
	if (m_type == FairyObjectType::Reference)
	{
		pRuntime->dec_ref(m_value.asReference.ownerTable);
	}
	m_type = FairyObjectType::Deleted;
}

long long FairyObject::toLong(Runtime* pRuntime)
{
	if (m_type == FairyObjectType::Int)
		return m_value.asLong;
	if (m_type == FairyObjectType::Bool)
		return m_value.asBool;
	if (m_type == FairyObjectType::String)
		return atoll(pRuntime->getStringTable().getString(m_value.asString));
	assert(!"Type is not covnertible into int");
}

stringId FairyObject::toStringId(Runtime* pRuntime)
{
	if (m_type == FairyObjectType::String)
		return m_value.asString;
	else
		return pRuntime->getStringTable().getStringId(toString(pRuntime).c_str());
}


std::string FairyObject::toString(Runtime* pRuntime)
{
	if (m_type == FairyObjectType::Int)
		return std::to_string(m_value.asLong);
	if (m_type == FairyObjectType::Bool)
		return m_value.asBool ? "true" : "false";
	if (m_type == FairyObjectType::String)
		return pRuntime->getStringTable().getString(m_value.asString);
	assert(!"Type is not covnertible into str");
}

bool FairyObject::toBool(Runtime* pRuntime)
{
	if (m_type == FairyObjectType::Int)
		return (bool)m_value.asLong;
	if (m_type == FairyObjectType::Bool)
		return m_value.asBool;
	if (m_type == FairyObjectType::String)
		return pRuntime->getStringTable().getStringId("") != m_value.asString;
	assert(!"Type is not covnertible into int");
}