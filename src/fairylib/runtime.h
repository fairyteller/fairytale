#pragma once
#include "fairy_object.h"
#include "auto_ref.h"
#include "config.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include <stack>
#include <list>
#include <set>
#include <memory>
#include <assert.h>
#include <sstream>

class FairytaleCore;
class ASTNode;

struct TraceLine
{
	const std::string* filename;
	int lineNo;
	stringId functionName;
};

class Runtime
{
public:
	Runtime(FairytaleCore* pCore);

	void set_stack_unwinding(bool value) { isStackUnwinding = value; }
	bool is_stack_unwinding() { return isStackUnwinding; }

	template<class T>
	ObjectRef safe_allocate(T value)
	{
		return ObjectRef(this, allocate(value));
	}

	template<class T>
	objectId allocate(T value)
	{
		assert(direct_memory_usage_semaphore == 0);
		if (freeIds.size())
		{
			objectId freeId = freeIds.top();
			allocatedObjects[freeId] = FairyObject(value);
			freeIds.pop();
			return freeId;
		}
		else
		{
			allocatedObjects.push_back(FairyObject(value));
			return allocatedObjects.size() - 1;
		}
	}

	template<class T>
	void allocate_on_stack(T value)
	{
		push_on_stack(allocate(value));
	}

	objectId copy_object(objectId original)
	{
		objectId newCopyId = allocate(FairyObjectType::Unknown);
		FairyObject* originalObj = getObject(original);
		FairyObject* newCopyObj = getObject(newCopyId);
		*newCopyObj = *originalObj;
		for (auto& row : newCopyObj->get_table())
		{
			inc_ref(row.second);
		}

		// we do not want to copy refCount, new objects starts with its own reference counter
		newCopyObj->refCount = 0;


		return newCopyId;
	}

	int execute_ast(std::unique_ptr<ASTNode>&& ast);

	std::unique_ptr<ASTNode> load_module(const std::string& filename);

	void import_module(const std::string& filename, stringId asTable)
	{
		execute_ast(std::move(load_module(filename)));
	}

	void deleteObject(objectId id)
	{
		getObject(id)->release(this);
		freeIds.push(id);
	}

	void assign_name_to_obj(objectId parentTable, const char* name, objectId object)
	{
		assign_name_to_obj(parentTable, getStringTable().getStringId(name), object);
	}
	void assign_name_to_obj(objectId parentTable, stringId sid, objectId object)
	{
		getObject(parentTable)->setattr(this, sid, object);
	}

	void push_on_stack(objectId id)
	{
		interpreterStack.push(id);
		inc_ref(id);
		if (gTRACE)
		{
			std::cout << "\nSTACK PROFILE: push " << id << std::endl;
			printStack();
		}
	}

	FairyObject* getObject(objectId id)
	{
		return &(allocatedObjects[id]);
	}

	ObjectRef safe_pop()
	{
		assert(stackFrames.top().objectStackTop < interpreterStack.size());
		objectId id = interpreterStack.top();
		interpreterStack.pop();
		ObjectRef result(this, id);
		dec_ref(id);
		return result;
	}

	ObjectRef safe_pop_and_dereference()
	{
		ObjectRef objectFromStack = safe_pop();
		return safe_dereference(objectFromStack);
	}

	objectId pop_from_stack_and_keep_reference()
	{
		assert(stackFrames.top().objectStackTop < interpreterStack.size());
		objectId id = interpreterStack.top();
		interpreterStack.pop();
		if (gTRACE)
		{
			std::cout << "\nSTACK PROFILE: soft pop " << id << std::endl;
			printStack();
		}
		return id;
	}

	void remove_top_from_stack()
	{
		assert(stackFrames.top().objectStackTop < interpreterStack.size());
		objectId id = interpreterStack.top();
		interpreterStack.pop();
		dec_ref(id);
		if (gTRACE)
		{
			std::cout << "\nSTACK PROFILE: pop " << id << std::endl;
			printStack();
		}
	}

	objectId soft_pop_and_deref_id_from_stack()
	{
		return dereference(pop_from_stack_and_keep_reference());
	}

	FairyObject* soft_pop_and_deref_object_from_stack()
	{
		return getObject(soft_pop_and_deref_id_from_stack());
	}

	objectId stack_top()
	{
		return interpreterStack.top();
	}

	void clear_stack()
	{
		auto& frame = stackFrames.top();
		while (interpreterStack.size() > frame.objectStackTop)
		{
			remove_top_from_stack();
		}
	}

	StringTable& getStringTable()
	{
		return m_stringTable;
	}

	objectId get_existing_object_or_allocate(objectId parentTable, stringId strId)
	{
		// TODO: fix two tables lookup and merge tables into one "localScope" table
		auto iter = getObject(parentTable)->get_table().find(strId);
		if (iter == getObject(parentTable)->get_table().end())
		{
			iter = getObject(globalScopeObject)->get_table().find(strId);
			if (iter == getObject(globalScopeObject)->get_table().end())
			{
				objectId id = allocate(FairyObjectType::Unknown);
				assign_name_to_obj(parentTable, strId, id);
				return id;
			}
			else
			{
				return iter->second;
			}
		}
		else
		{
			return iter->second;
		}
	}

	objectId dereference(objectId oid)
	{
		FairyObject* pObj = getObject(oid);
		objectId result = oid;
		if (pObj->getType() == FairyObjectType::Reference)
		{
			result = getObject(pObj->asReference().ownerTable)->getattr(this, pObj->asReference().attributeKey);
		}
		inc_ref(result); // maybe leak
		return result;
	}

	ObjectRef safe_dereference(const ObjectRef& objectRef)
	{
		ObjectRef result = objectRef;
		if (objectRef->getType() == FairyObjectType::Reference)
		{
			result.reset(getObject(objectRef->asReference().ownerTable)->getattr(this, objectRef->asReference().attributeKey));
		}
		return result;
	}

	template<class T>
	objectId set_member(objectId parentId, const char* name, T value)
	{
		stringId sid = getStringTable().getStringId(name);
		ObjectRef valueRef = safe_allocate(value);
		assign_name_to_obj(parentId, sid, valueRef.id());
		return valueRef.id();
	}
	objectId register_member_function(objectId parentId, const char* name, WrappedFunction fn)
	{
		return set_member(parentId, name, fn);
	}
	objectId register_function_in_module(objectId moduleId, const char* name, WrappedFunction fn)
	{
		assert(getObject(moduleId)->getType() == FairyObjectType::Module);
		return register_member_function(moduleId, name, fn);
	}

	objectId register_global_function(const char* name, WrappedFunction fn)
	{
		return register_function_in_module(globalScopeObject, name, fn);
	}

	void call_method(objectId id, stringId methodId);
	void call(objectId id, objectId context = -1);

	void printStack()
	{
		std::cout << "Stack Values: ";
		std::stack<objectId> tmp;
		while (!interpreterStack.empty())
		{
			objectId id = interpreterStack.top();
			tmp.push(id);
			FairyObject* pObj = &(allocatedObjects[id]);
			std::cout << "#" << id << ":" << pObj->describe() << " ";;
			interpreterStack.pop();
		}

		while (!tmp.empty())
		{
			interpreterStack.push(tmp.top());
			tmp.pop();
		}
		std::cout << std::endl;
	}

	void inc_ref(objectId id)
	{
		getObject(id)->refCount++;
	}

	void dec_ref(objectId id)
	{
		assert(getObject(id)->refCount > 0);
		getObject(id)->refCount--;
		if (getObject(id)->refCount == 0)
			deleteObject(id);
	}

	void trace_memory()
	{
		std::cout << "\nTrace:\n";
		int i = 0;
		for (auto& obj : allocatedObjects)
		{
			std::cout << i++ << ") " << obj.describe() << "\n";
		}
		std::cout << std::endl;
	}

	void trace_names()
	{
		std::cout << "\nTrace Names:\n";
		for (auto& obj : getObject(globalScopeObject)->get_table())
		{
			std::cout << getStringTable().getString(obj.first) << " - " << getObject(obj.second)->describe() << "\n";
		}
		std::cout << std::endl;
	}

	void push_frame()
	{
		stackFrames.push({ interpreterStack.size(), nameUsageStack.size() });
	}

	void set_arguments_amount(size_t args_amount)
	{
		stackFrames.top().nameUsageStackTop -= args_amount;
	}

	void pop_frame()
	{
		auto& frame = stackFrames.top();
		while (interpreterStack.size() > frame.objectStackTop)
		{
			remove_top_from_stack();
		}
		while (nameUsageStack.size() > frame.nameUsageStackTop)
		{
			//			unbind_name(nameUsageStack.top());
			nameUsageStack.pop();
		}

		stackFrames.pop();
	}

	size_t get_amount_of_objects_in_stack_frame()
	{
		return interpreterStack.size() - stackFrames.top().objectStackTop;
	}

	bool is_frame_stack_empty()
	{
		return stackFrames.top().objectStackTop == interpreterStack.size();
	}

	void save_to_register(objectId id)
	{
		if (regId != -1)
			load_from_register();
		regId = id;
		inc_ref(id);
	}

	objectId copy_from_register()
	{
		return regId;
	}

	objectId load_from_register()
	{
		objectId id = regId;
		dec_ref(id);
		regId = -1;
		return id;
	}

	objectId get_global_scope()
	{
		return globalScopeObject;
	}

	void set_exception_as_handled()
	{
		isExceptionRaised = false;
		isStackUnwinding = false;
	}
	bool is_exception_raised()
	{
		return isExceptionRaised;
	}
	void throw_exception(objectId exceptionObject)
	{
		save_to_register(exceptionObject);
		savedStackTrace = currentStackTrace;
		isStackUnwinding = true;
		isExceptionRaised = true;
	}

	void throw_runtime_error(const std::string& errorText)
	{
		//In future it will be: std::string result = errorText +getStackTrace();
		objectId exception = allocate(getStringTable().getStringId(errorText));
		throw_exception(exception);
	}

	void push_trace_line(TraceLine traceLine)
	{
		currentStackTrace.push(traceLine);
	}

	void pop_trace_line()
	{
		currentStackTrace.pop();
	}

	std::string get_last_stack_trace();
	
	size_t direct_memory_usage_semaphore;
private:
	struct StackFrame
	{
		size_t objectStackTop;
		size_t nameUsageStackTop;
	};

	StringTable m_stringTable;
	std::vector<FairyObject> allocatedObjects;
	std::stack<objectId> freeIds;
	//FairyTable globalObjectTable;
	objectId globalScopeObject;
	std::stack<objectId> interpreterStack;
	std::stack<stringId> nameUsageStack;
	std::stack<TraceLine> currentStackTrace;
	std::stack<TraceLine> savedStackTrace;
	std::stack<StackFrame> stackFrames;
	bool isStackUnwinding = false;
	bool isExceptionRaised = false;
	objectId regId = -1;

	std::vector<std::unique_ptr<ASTNode>> loadedModuleASTs;
	FairytaleCore* m_core;
};