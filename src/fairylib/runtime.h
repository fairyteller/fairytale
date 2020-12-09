#pragma once
#include "fairy_object.h"
#include "auto_ref.h"
#include <iostream>
#include <vector>
#include <string>
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

class Runtime
{
public:
	Runtime(FairytaleCore* pCore);

	void set_unwinding(bool value) { unwinding = value; }
	bool is_unwinding() { return unwinding; }
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

	void assign_name_to_obj(objectId parentTable, stringId sid, objectId od)
	{
		getObject(parentTable)->setattr(this, sid, od);
	}

	//	void unbind_name(stringId strId)
	//	{
	//		dec_ref(getObject(globalScopeObject)->get_table()[strId]);
	//		getObject(globalScopeObject)->get_table().erase(strId);
	//	}

	template<class T>
	void put_literal_on_stack(T value)
	{
		objectId id = allocate(value);
		push_on_stack(id);
	}

	void push_on_stack(objectId id)
	{
		interpreterStack.push(id);
		inc_ref(id);
		if (g_trace)
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
		assert(stackFrames.top().objectStackTop < interpreterStack.size());
		objectId id = interpreterStack.top();
		interpreterStack.pop();
		ObjectRef objectFromStack(this, id);
		dec_ref(id);
		return safe_dereference(objectFromStack);
	}

	// TODO: check if we have memory leak here
	objectId soft_pop_from_stack()
	{
		assert(stackFrames.top().objectStackTop < interpreterStack.size());
		objectId id = interpreterStack.top();
		interpreterStack.pop();
		if (g_trace)
		{
			std::cout << "\nSTACK PROFILE: soft pop " << id << std::endl;
			printStack();
		}
		return id;
	}

	objectId pop_from_stack()
	{
		assert(stackFrames.top().objectStackTop < interpreterStack.size());
		objectId id = interpreterStack.top();
		interpreterStack.pop();
		dec_ref(id);
		if (g_trace)
		{
			std::cout << "\nSTACK PROFILE: pop " << id << std::endl;
			printStack();
		}
		return id;
	}



	objectId pop_and_deref_id_from_stack()
	{
		return dereference(pop_from_stack());
	}

	FairyObject* pop_and_deref_object_from_stack()
	{
		return getObject(pop_and_deref_id_from_stack());
	}

	objectId soft_pop_and_deref_id_from_stack()
	{
		return dereference(soft_pop_from_stack());
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
			pop_from_stack();
		}
	}
	FairyObject* pop_object_from_stack()
	{
		return getObject(pop_from_stack());
	}

	StringTable& getStringTable()
	{
		return m_stringTable;
	}

	objectId get_existing_object_or_allocate(objectId parentTable, stringId strId)
	{
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

	objectId register_global_function(const char* name, WrappedFunction fn)
	{
		stringId sid = getStringTable().getStringId(name);
		objectId functionId = allocate(fn);
		assign_name_to_obj(globalScopeObject, sid, functionId);
		return functionId;
	}

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

	template<class T>
	T pop()
	{
		static_assert("this type is not implemented");
	}
	template<>
	long long pop<long long>()
	{
		return pop_and_deref_object_from_stack()->asLong();
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
			pop_from_stack();
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

	objectId getGlobalScope()
	{
		return globalScopeObject;
	}
private:
	struct StackFrame
	{
		size_t objectStackTop;
		size_t nameUsageStackTop;
	};
	size_t direct_memory_usage_semaphore;
	StringTable m_stringTable;
	std::vector<FairyObject> allocatedObjects;
	std::stack<objectId> freeIds;
	//FairyTable globalObjectTable;
	objectId globalScopeObject;
	std::stack<objectId> interpreterStack;
	std::stack<stringId> nameUsageStack;
	std::stack<StackFrame> stackFrames;
	bool unwinding = false;
	objectId regId = -1;

	std::vector<std::unique_ptr<ASTNode>> loadedModuleASTs;
	FairytaleCore* m_core;
};