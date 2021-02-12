#pragma once
#include "standard_library.h"
#include "abstract_syntax_tree.h"
#include <cstring>

void sum_wrapper(Runtime* pRuntime, objectId context)
{
	ObjectRef b = pRuntime->safe_pop_and_dereference();
	ObjectRef a = pRuntime->safe_pop_and_dereference();
	assert(b->getType() == a->getType());
	if (a->getType() == FairyObjectType::Int)
	{
		long long summed = a->asLong() + b->asLong();
		pRuntime->allocate_on_stack(summed);
	}
	if (a->getType() == FairyObjectType::Float)
	{
		double summed = a->asDouble() + b->asDouble();
		pRuntime->allocate_on_stack(summed);
	}
	else if (a->getType() == FairyObjectType::String)
	{

		std::string summed{ pRuntime->getStringTable().getString(a->asString()) };
		summed += pRuntime->getStringTable().getString(b->asString());
		pRuntime->allocate_on_stack(pRuntime->getStringTable().getStringId(summed.c_str()));
	}
}

void sub_wrapper(Runtime* pRuntime, objectId context)
{
	ObjectRef b = pRuntime->safe_pop_and_dereference();
	ObjectRef a = pRuntime->safe_pop_and_dereference();
	assert(b->getType() == a->getType());
	if (a->getType() == FairyObjectType::Int)
	{
		long long summed = a->asLong() - b->asLong();
		pRuntime->allocate_on_stack(summed);
	}
	if (a->getType() == FairyObjectType::Float)
	{
		double summed = a->asDouble() - b->asDouble();
		pRuntime->allocate_on_stack(summed);
	}
}

void mul_wrapper(Runtime* pRuntime, objectId context)
{
	ObjectRef b = pRuntime->safe_pop_and_dereference();
	ObjectRef a = pRuntime->safe_pop_and_dereference();
	assert(b->getType() == a->getType());
	if (a->getType() == FairyObjectType::Int)
	{
		long long summed = a->asLong() * b->asLong();
		pRuntime->allocate_on_stack(summed);
	}
	if (a->getType() == FairyObjectType::Float)
	{
		double summed = a->asDouble() * b->asDouble();
		pRuntime->allocate_on_stack(summed);
	}
}

void div_wrapper(Runtime* pRuntime, objectId context)
{
	ObjectRef b = pRuntime->safe_pop_and_dereference();
	ObjectRef a = pRuntime->safe_pop_and_dereference();
	assert(b->getType() == a->getType());
	if (a->getType() == FairyObjectType::Int)
	{
		long long summed = a->asLong() / b->asLong();
		pRuntime->allocate_on_stack(summed);
	}
	if (a->getType() == FairyObjectType::Float)
	{
		double summed = a->asDouble() / b->asDouble();
		pRuntime->allocate_on_stack(summed);
	}
}

void mod_wrapper(Runtime* pRuntime, objectId context)
{
	ObjectRef b = pRuntime->safe_pop_and_dereference();
	ObjectRef a = pRuntime->safe_pop_and_dereference();
	assert(b->getType() == a->getType());
	if (a->getType() == FairyObjectType::Int)
	{
		long long summed = a->asLong() % b->asLong();
		pRuntime->allocate_on_stack(summed);
	}
}

void pow_wrapper(Runtime* pRuntime, objectId context)
{
	ObjectRef b = pRuntime->safe_pop_and_dereference();
	ObjectRef a = pRuntime->safe_pop_and_dereference();
	assert(b->getType() == a->getType());
	if (a->getType() == FairyObjectType::Int)
	{
		long long summed = pow(a->asLong(), b->asLong());
		pRuntime->allocate_on_stack(summed);
	}
	if (a->getType() == FairyObjectType::Float)
	{
		double summed = pow(a->asDouble(), b->asDouble());
		pRuntime->allocate_on_stack(summed);
	}
}

void assign(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop();
	ObjectRef lhsDeref = pRuntime->safe_dereference(lhs);
	
	if (lhs->getType() != FairyObjectType::Reference)
	{
		pRuntime->throw_runtime_error("Assignment left side should be a reference");
		return;
	}
	if (rhs->getType() == FairyObjectType::FairyFunction)
	{
		pRuntime->setattr(rhs.id(), pRuntime->getStringTable().getStringId("traceName"), pRuntime->allocate(lhs->asReference().attributeKey));
	}
	pRuntime->setattr(lhs->asReference().ownerTable, lhs->asReference().attributeKey, rhs.id());
	pRuntime->push_on_stack(lhs.id());
}

void sum_compound(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop();
	ObjectRef lhsDeref = pRuntime->safe_dereference(lhs);
	pRuntime->push_on_stack(lhsDeref.id());
	pRuntime->push_on_stack(rhs.id());
	sum_wrapper(pRuntime, context);
	ObjectRef opResult = pRuntime->safe_pop();
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->setattr(lhs->asReference().ownerTable, lhs->asReference().attributeKey, opResult.id());
	pRuntime->push_on_stack(lhs.id());
}

void sub_compound(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop();
	ObjectRef lhsDeref = pRuntime->safe_dereference(lhs);
	pRuntime->push_on_stack(lhsDeref.id());
	pRuntime->push_on_stack(rhs.id());
	sub_wrapper(pRuntime, context);
	ObjectRef opResult = pRuntime->safe_pop();
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->setattr(lhs->asReference().ownerTable, lhs->asReference().attributeKey, opResult.id());
	pRuntime->push_on_stack(lhs.id());
}


void mul_compound(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop();
	ObjectRef lhsDeref = pRuntime->safe_dereference(lhs);
	pRuntime->push_on_stack(lhsDeref.id());
	pRuntime->push_on_stack(rhs.id());
	mul_wrapper(pRuntime, context);
	ObjectRef opResult = pRuntime->safe_pop();
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->setattr(lhs->asReference().ownerTable, lhs->asReference().attributeKey, opResult.id());
	pRuntime->push_on_stack(lhs.id());
}


void div_compound(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop();
	ObjectRef lhsDeref = pRuntime->safe_dereference(lhs);
	pRuntime->push_on_stack(lhsDeref.id());
	pRuntime->push_on_stack(rhs.id());
	div_wrapper(pRuntime, context);
	ObjectRef opResult = pRuntime->safe_pop();
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->setattr(lhs->asReference().ownerTable, lhs->asReference().attributeKey, opResult.id());
	pRuntime->push_on_stack(lhs.id());
}


void mod_compound(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop();
	ObjectRef lhsDeref = pRuntime->safe_dereference(lhs);
	pRuntime->push_on_stack(lhsDeref.id());
	pRuntime->push_on_stack(rhs.id());
	mod_wrapper(pRuntime, context);
	ObjectRef opResult = pRuntime->safe_pop();
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->setattr(lhs->asReference().ownerTable, lhs->asReference().attributeKey, opResult.id());
	pRuntime->push_on_stack(lhs.id());
}


void __is_eq__(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop_and_dereference();
	pRuntime->allocate_on_stack(*lhs == *rhs);
}

void __is_neq__(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop_and_dereference();
	pRuntime->allocate_on_stack(*lhs != *rhs);
}

void __less_than__(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop_and_dereference();
	pRuntime->allocate_on_stack(*lhs < *rhs);
}

void __more_than__(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop_and_dereference();
	pRuntime->allocate_on_stack(*lhs > *rhs);
}

void __less_eq_than__(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop_and_dereference();
	pRuntime->allocate_on_stack(*lhs <= *rhs);
}

void __more_eq_than__(Runtime* pRuntime, objectId context)
{
	ObjectRef rhs = pRuntime->safe_pop_and_dereference();
	ObjectRef lhs = pRuntime->safe_pop_and_dereference();
	pRuntime->allocate_on_stack(*lhs >= *rhs);
}

void __minus_prefix__(Runtime* pRuntime, objectId context)
{
	ObjectRef arg = pRuntime->safe_pop_and_dereference();
	if (arg->getType() == FairyObjectType::Int)
	{
		pRuntime->allocate_on_stack(-arg->asLong());
		return;
	}
	else if (arg->getType() == FairyObjectType::Float)
	{
		pRuntime->allocate_on_stack(-arg->asDouble());
		return;
	}
	pRuntime->throw_runtime_error("Object " + std::to_string(arg.id()) + " should have a type Int");
}

void print_wrapper(Runtime* pRuntime, objectId context)
{
	ObjectRef arg = pRuntime->safe_pop_and_dereference();
	stringId a = arg->toStringId(pRuntime);
	printf("%s\n", pRuntime->getStringTable().getString(a));
}

char* input()
{
	char buf[1024];
	scanf("%s", &buf);
	char* ptr = new char[strlen(buf)];
	strcpy(ptr, buf);
	return ptr;
}

void input_wrapper(Runtime* pRuntime, objectId context)
{
	stringId sid = pRuntime->getStringTable().getStringId(input());
	pRuntime->allocate_on_stack(sid);
}

void trace(Runtime* pRuntime, objectId context)
{
	pRuntime->trace_memory();
}

void trace_names(Runtime* pRuntime, objectId context)
{
	pRuntime->trace_names();
}

void print_stack(Runtime* pRuntime, objectId context)
{
	pRuntime->printStack();
}

void int_wrapper(Runtime* pRuntime, objectId context)
{
	ObjectRef arg = pRuntime->safe_pop_and_dereference();
	pRuntime->allocate_on_stack(arg->toLong(pRuntime));
}

void str_wrapper(Runtime* pRuntime, objectId context)
{
	ObjectRef arg = pRuntime->safe_pop_and_dereference();
	pRuntime->allocate_on_stack(arg->toStringId(pRuntime));
}

void bool_wrapper(Runtime* pRuntime, objectId context)
{
	ObjectRef arg = pRuntime->safe_pop_and_dereference();
	pRuntime->allocate_on_stack(arg->toBool(pRuntime));
}

void import_module(Runtime* pRuntime, objectId context)
{
	ObjectRef module_name = pRuntime->safe_pop_and_dereference();
	std::string filename = module_name->toString(pRuntime);
	pRuntime->import_module(filename, -1);
}

// attrobj = getattr(obj, "key")
void getattr(Runtime* pRuntime, objectId context)
{
	ObjectRef key = pRuntime->safe_pop_and_dereference();
	ObjectRef table = pRuntime->safe_pop_and_dereference();
	pRuntime->push_on_stack(pRuntime->getattr(table.id(), key->asString()));
}

// setattr(obj, "key", value)
void setattr(Runtime* pRuntime, objectId context)
{
	ObjectRef value = pRuntime->safe_pop_and_dereference();
	ObjectRef key = pRuntime->safe_pop_and_dereference();
	ObjectRef table = pRuntime->safe_pop_and_dereference();
	pRuntime->setattr(table.id(), key->asString(), value.id());
}

// attrobj = getattr_direct(obj, "key")
void getattr_direct(Runtime* pRuntime, objectId context)
{
	ObjectRef key = pRuntime->safe_pop_and_dereference();
	ObjectRef table = pRuntime->safe_pop_and_dereference();
	pRuntime->push_on_stack(table->getattr_internal(pRuntime, key->asString()));
}

// setattr_direct(obj, "key", value)
void setattr_direct(Runtime* pRuntime, objectId context)
{
	ObjectRef value = pRuntime->safe_pop_and_dereference();
	ObjectRef key = pRuntime->safe_pop_and_dereference();
	ObjectRef table = pRuntime->safe_pop_and_dereference();
	table->setattr_internal(pRuntime, key->asString(), value.id());
}

// size = arr.size()
void array_size(Runtime* pRuntime, objectId context)
{
	ObjectRef self(pRuntime, context);
	pRuntime->push_on_stack(pRuntime->getattr(self.id(), pRuntime->getStringTable().getStringId("__size")));
}

// size = arr.unroll()
void array_unroll(Runtime* pRuntime, objectId context)
{
	ObjectRef self(pRuntime, context);
	ObjectRef sizeObj(pRuntime, pRuntime->getattr(self.id(), pRuntime->getStringTable().getStringId("__size")));
	long long size = sizeObj->asLong();
	for (int i = 0; i < size; ++i)
	{
		pRuntime->push_on_stack(pRuntime->getattr(self.id(), pRuntime->getStringTable().getStringId(i)));
	}
}

void array_get(Runtime* pRuntime, objectId context)
{
	ObjectRef self(pRuntime, context);
	ObjectRef key = pRuntime->safe_pop_and_dereference();
	long long index = key->asLong();
	pRuntime->push_on_stack(pRuntime->getattr(self.id(), pRuntime->getStringTable().getStringId(index)));
}

void array_index(Runtime* pRuntime, objectId context)
{
	ObjectRef indexObj = pRuntime->safe_pop_and_dereference();
	long long index = indexObj->asLong();
	pRuntime->allocate_on_stack(FairyReference{ context, pRuntime->getStringTable().getStringId(index) });
	pRuntime->inc_ref(context);
}

// obj = array(arg0, arg1, arg2, ..., argn);
void array(Runtime* pRuntime, objectId context)
{
	ObjectRef newArray(pRuntime, pRuntime->allocate(FairyObjectType::Array));
	assert(pRuntime->get_amount_of_objects_in_stack_frame() > 0);
	int amountOfArguments = pRuntime->get_amount_of_objects_in_stack_frame() - 1; //we do not want to pull "self" object
	int remainingObjects = amountOfArguments;
	while (remainingObjects)
	{

		ObjectRef arg = pRuntime->safe_pop_and_dereference();
		--remainingObjects;
		stringId sid = pRuntime->getStringTable().getStringId(remainingObjects); // index is equal to amount of objects-1
		pRuntime->setattr(newArray.id(), sid, arg.id());
	}
	objectId __sizeObjId = pRuntime->allocate((long long)amountOfArguments);
	pRuntime->setattr(newArray.id(), pRuntime->getStringTable().getStringId("__size"), __sizeObjId);
	objectId sizeObjId = pRuntime->allocate(array_size);
	pRuntime->setattr(newArray.id(), pRuntime->getStringTable().getStringId("size"), sizeObjId);
	objectId getObjId = pRuntime->allocate(array_get);
	pRuntime->setattr(newArray.id(), pRuntime->getStringTable().getStringId("get"), getObjId);
	objectId index = pRuntime->allocate(array_index);
	pRuntime->setattr(newArray.id(), pRuntime->getStringTable().getStringId("__index__"), index);
	objectId unrollObjId = pRuntime->allocate(array_unroll);
	pRuntime->setattr(newArray.id(), pRuntime->getStringTable().getStringId("unroll"), unrollObjId);

	pRuntime->push_on_stack(newArray.id());
}

void object(Runtime* pRuntime, objectId context)
{
	pRuntime->allocate_on_stack(FairyObjectType::Object);
}