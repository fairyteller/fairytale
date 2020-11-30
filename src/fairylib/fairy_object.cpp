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
	return pRuntime->allocate_empty();
}