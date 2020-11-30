#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <list>
#include <set>
#include <memory>
#include <assert.h>

class Runtime;
class FuncASTN;
class FairytaleCore;
class ASTNode;
typedef size_t objectId;
typedef void (*WrappedFunction)(Runtime*, objectId);


extern bool g_trace;
typedef size_t stringId;


typedef std::unordered_map<stringId, objectId> FairyTable;

enum class FairyObjectType
{
	Unknown,
	Int,
	Float,
	Bool,
	String,
	WrappedFunction,
	FairyFunction,
	Class,
};

union FairyValue
{
	long long asLong;
	double asDouble;
	bool asBool;
	WrappedFunction asWrappedFunction;
	FuncASTN* asFairyFunction;
	stringId asString;
};

class FairyObject
{
public:
	FairyObject()
	{
		m_value.asLong = 0;
		m_type = FairyObjectType::Unknown;
	}
	FairyObject(long long value)
	{
		m_value.asLong = value;
		m_type = FairyObjectType::Int;
	}
	FairyObject(bool value)
	{
		m_value.asBool = value;
		m_type = FairyObjectType::Bool;
	}
	FairyObject(WrappedFunction fn)
	{
		m_value.asWrappedFunction = fn;
		m_type = FairyObjectType::WrappedFunction;
	}
	FairyObject(FuncASTN* fn)
	{
		m_value.asFairyFunction = fn;
		m_type = FairyObjectType::FairyFunction;
	}
	FairyObject(stringId strId)
	{
		m_value.asString = strId;
		m_type = FairyObjectType::String;
	}

	const FairyObject& operator=(const FairyObject& rhs)
	{
		this->m_type = rhs.m_type;
		this->m_value = rhs.m_value;
		this->table = rhs.table;
		return *this;
	}

	void operator+=(FairyObject& rhs)
	{
		assert(m_type == rhs.m_type);
		if (m_type == FairyObjectType::Int)
		{
			m_value.asLong += rhs.m_value.asLong;
		}
	}

	void operator-=(FairyObject& rhs)
	{
		assert(m_type == rhs.m_type);
		if (m_type == FairyObjectType::Int)
		{
			m_value.asLong -= rhs.m_value.asLong;
		}
	}

	void operator*=(FairyObject& rhs)
	{
		assert(m_type == rhs.m_type);
		if (m_type == FairyObjectType::Int)
		{
			m_value.asLong *= rhs.m_value.asLong;
		}
	}

	void operator/=(FairyObject& rhs)
	{
		assert(m_type == rhs.m_type);
		if (m_type == FairyObjectType::Int)
		{
			m_value.asLong /= rhs.m_value.asLong;
		}
	}

	void operator%=(FairyObject& rhs)
	{
		assert(m_type == rhs.m_type);
		if (m_type == FairyObjectType::Int)
		{
			m_value.asLong %= rhs.m_value.asLong;
		}
	}

	bool operator==(FairyObject& rhs)
	{
		if (m_type != rhs.m_type)
			return false;

		if (m_type == FairyObjectType::Bool)
		{
			if (m_value.asBool == rhs.m_value.asBool)
			{
				return true;
			}
		}
		else if (m_type == FairyObjectType::Int)
		{
			if (m_value.asLong == rhs.m_value.asLong)
			{
				return true;
			}
		}
		else if (m_type == FairyObjectType::String)
		{
			if (m_value.asString == rhs.m_value.asString)
			{
				return true;
			}
		}
		return false;
	}
	bool operator<(FairyObject& rhs)
	{
		assert(m_type == rhs.m_type);

		if (m_type == FairyObjectType::Bool)
		{
			if (m_value.asBool < rhs.m_value.asBool)
			{
				return true;
			}
		}
		else if (m_type == FairyObjectType::Int)
		{
			if (m_value.asLong < rhs.m_value.asLong)
			{
				return true;
			}
		}
		return false;
	}
	bool operator>(FairyObject& rhs)
	{
		assert(m_type == rhs.m_type);

		if (m_type == FairyObjectType::Bool)
		{
			if (m_value.asBool > rhs.m_value.asBool)
			{
				return true;
			}
		}
		else if (m_type == FairyObjectType::Int)
		{
			if (m_value.asLong > rhs.m_value.asLong)
			{
				return true;
			}
		}
		return false;
	}
	bool operator!=(FairyObject& rhs)
	{
		return !(*this == rhs);
	}
	bool operator<=(FairyObject& rhs)
	{
		return !(*this > rhs);
	}
	bool operator>=(FairyObject& rhs)
	{
		return !(*this < rhs);
	}
	void assign(Runtime* pRuntime, FairyObject* rhs);

	long long toLong(Runtime* pRuntime);

	stringId toStringId(Runtime* pRuntime);

	std::string toString(Runtime* pRuntime);

	bool toBool(Runtime* pRuntime);

	WrappedFunction asWrappedFunction()
	{
		assert(m_type == FairyObjectType::WrappedFunction);
		return m_value.asWrappedFunction;
	}
	FuncASTN* asFairyFunction()
	{
		assert(m_type == FairyObjectType::FairyFunction);
		return m_value.asFairyFunction;
	}
	bool asBool()
	{
		assert(m_type == FairyObjectType::Bool);
		return m_value.asBool;
	}
	long long asLong()
	{
		assert(m_type == FairyObjectType::Int);
		return m_value.asLong;
	}
	stringId asString()
	{
		assert(m_type == FairyObjectType::String);
		return m_value.asString;
	}
	FairyObjectType getType() { return m_type; }

	void release(Runtime* pRuntime);

	~FairyObject()
	{
	}

	std::string describe();

	void setattr(Runtime* pRuntime, stringId strid, objectId objid);
	objectId getattr(Runtime* pRuntime, stringId key);

	const FairyTable& get_table()
	{
		return table;
	}

	int refCount = 0;
private:
	FairyObjectType m_type;
	FairyValue m_value;
	FairyTable table;
};