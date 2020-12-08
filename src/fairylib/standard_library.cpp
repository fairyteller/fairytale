#pragma once
#include "standard_library.h"

long long sum(long long a, long long b)
{
	return a + b;
}

void sum_wrapper(Runtime* pRuntime, objectId context)
{
	FairyObject* b = pRuntime->pop_and_deref_object_from_stack();
	FairyObject* a = pRuntime->pop_and_deref_object_from_stack();
	assert(b->getType() == a->getType());
	if (a->getType() == FairyObjectType::Int)
	{
		long long summed = a->asLong() + b->asLong();
		objectId result = pRuntime->allocate(summed);
		pRuntime->push_on_stack(result);
	}
	else if (a->getType() == FairyObjectType::String)
	{

		std::string summed{ pRuntime->getStringTable().getString(a->asString()) };
		summed += pRuntime->getStringTable().getString(b->asString());
		objectId result = pRuntime->allocate(pRuntime->getStringTable().getStringId(summed.c_str()));
		pRuntime->push_on_stack(result);
	}
}

long long sub(long long a, long long b)
{
	return a - b;
}

void sub_wrapper(Runtime* pRuntime, objectId context)
{
	long long b = pRuntime->pop_and_deref_object_from_stack()->asLong();
	long long a = pRuntime->pop_and_deref_object_from_stack()->asLong();
	objectId result = pRuntime->allocate(sub(a, b));
	pRuntime->push_on_stack(result);
}

long long mul(long long a, long long b)
{
	return a * b;
}

void mul_wrapper(Runtime* pRuntime, objectId context)
{
	long long b = pRuntime->pop_and_deref_object_from_stack()->asLong();
	long long a = pRuntime->pop_and_deref_object_from_stack()->asLong();
	objectId result = pRuntime->allocate(mul(a, b));
	pRuntime->push_on_stack(result);
}

long long __div__(long long a, long long b)
{
	return a / b;
}

long long __mod__(long long a, long long b)
{
	return a % b;
}

void div_wrapper(Runtime* pRuntime, objectId context)
{
	long long b = pRuntime->pop_and_deref_object_from_stack()->asLong();
	long long a = pRuntime->pop_and_deref_object_from_stack()->asLong();
	objectId result = pRuntime->allocate(__div__(a, b));
	pRuntime->push_on_stack(result);
}

long long pow(long long a, long long b)
{
	long long result = 1;
	while (b-- > 0)
	{
		result *= a;
	}
	return result;
}

void pow_wrapper(Runtime* pRuntime, objectId context)
{
	long long b = pRuntime->pop_and_deref_object_from_stack()->asLong();
	long long a = pRuntime->pop_and_deref_object_from_stack()->asLong();
	objectId result = pRuntime->allocate(pow(a, b));
	pRuntime->push_on_stack(result);
}

void assign(Runtime* pRuntime, objectId context)
{
	objectId rhsId = pRuntime->soft_pop_from_stack();
	objectId rhsValue = pRuntime->dereference(rhsId);
	objectId lhsId = pRuntime->soft_pop_from_stack();
	FairyObject* lhs = pRuntime->getObject(lhsId);
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->getObject(lhs->asReference().ownerTable)->setattr(pRuntime, lhs->asReference().attributeKey, rhsValue);
	pRuntime->push_on_stack(lhsId);
	pRuntime->dec_ref(rhsId);
	pRuntime->dec_ref(lhsId);
}

void sum_compound(Runtime* pRuntime, objectId context)
{
	objectId rhsId = pRuntime->soft_pop_and_deref_id_from_stack();
	objectId lhsId = pRuntime->soft_pop_from_stack();

	pRuntime->push_on_stack(pRuntime->dereference(lhsId));
	pRuntime->push_on_stack(rhsId);
	sum_wrapper(pRuntime, context);
	objectId sumResult = pRuntime->soft_pop_from_stack();

	FairyObject* rhs = pRuntime->getObject(rhsId);
	FairyObject* lhs = pRuntime->getObject(lhsId);
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->getObject(lhs->asReference().ownerTable)->setattr(pRuntime, lhs->asReference().attributeKey, sumResult);
	pRuntime->push_on_stack(lhsId);
	pRuntime->dec_ref(rhsId);
	pRuntime->dec_ref(sumResult);
	pRuntime->dec_ref(lhsId);
}

void sub_compound(Runtime* pRuntime, objectId context)
{
	objectId rhsId = pRuntime->soft_pop_and_deref_id_from_stack();
	objectId lhsId = pRuntime->soft_pop_from_stack();

	pRuntime->push_on_stack(pRuntime->dereference(lhsId));
	pRuntime->push_on_stack(rhsId);
	sub_wrapper(pRuntime, context);
	objectId sumResult = pRuntime->soft_pop_from_stack();

	FairyObject* rhs = pRuntime->getObject(rhsId);
	FairyObject* lhs = pRuntime->getObject(lhsId);
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->getObject(lhs->asReference().ownerTable)->setattr(pRuntime, lhs->asReference().attributeKey, sumResult);
	pRuntime->push_on_stack(lhsId);
	pRuntime->dec_ref(rhsId);
	pRuntime->dec_ref(sumResult);
	pRuntime->dec_ref(lhsId);
}


void mul_compound(Runtime* pRuntime, objectId context)
{
	objectId rhsId = pRuntime->soft_pop_and_deref_id_from_stack();
	objectId lhsId = pRuntime->soft_pop_from_stack();

	pRuntime->push_on_stack(pRuntime->dereference(lhsId));
	pRuntime->push_on_stack(rhsId);
	mul_wrapper(pRuntime, context);
	objectId sumResult = pRuntime->soft_pop_from_stack();

	FairyObject* rhs = pRuntime->getObject(rhsId);
	FairyObject* lhs = pRuntime->getObject(lhsId);
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->getObject(lhs->asReference().ownerTable)->setattr(pRuntime, lhs->asReference().attributeKey, sumResult);
	pRuntime->push_on_stack(lhsId);
	pRuntime->dec_ref(rhsId);
	pRuntime->dec_ref(sumResult);
	pRuntime->dec_ref(lhsId);
}


void div_compound(Runtime* pRuntime, objectId context)
{
	objectId rhsId = pRuntime->soft_pop_and_deref_id_from_stack();
	objectId lhsId = pRuntime->soft_pop_from_stack();

	pRuntime->push_on_stack(pRuntime->dereference(lhsId));
	pRuntime->push_on_stack(rhsId);
	div_wrapper(pRuntime, context);
	objectId sumResult = pRuntime->soft_pop_from_stack();

	FairyObject* rhs = pRuntime->getObject(rhsId);
	FairyObject* lhs = pRuntime->getObject(lhsId);
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->getObject(lhs->asReference().ownerTable)->setattr(pRuntime, lhs->asReference().attributeKey, sumResult);
	pRuntime->push_on_stack(lhsId);
	pRuntime->dec_ref(rhsId);
	pRuntime->dec_ref(sumResult);
	pRuntime->dec_ref(lhsId);
}


void mod_compound(Runtime* pRuntime, objectId context)
{
	objectId rhsId = pRuntime->soft_pop_and_deref_id_from_stack();
	objectId lhsId = pRuntime->soft_pop_from_stack();

	pRuntime->push_on_stack(pRuntime->dereference(lhsId));
	pRuntime->push_on_stack(rhsId);
	wrap<long long, long long, long long, __mod__>()(pRuntime, context);
	objectId sumResult = pRuntime->soft_pop_from_stack();

	FairyObject* rhs = pRuntime->getObject(rhsId);
	FairyObject* lhs = pRuntime->getObject(lhsId);
	assert(lhs->getType() == FairyObjectType::Reference);
	pRuntime->getObject(lhs->asReference().ownerTable)->setattr(pRuntime, lhs->asReference().attributeKey, sumResult);
	pRuntime->push_on_stack(lhsId);
	pRuntime->dec_ref(rhsId);
	pRuntime->dec_ref(sumResult);
	pRuntime->dec_ref(lhsId);
}



void __is_eq__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->pop_and_deref_object_from_stack();
	FairyObject* lhs = pRuntime->pop_and_deref_object_from_stack();
	objectId result = pRuntime->allocate(*lhs == *rhs);
	pRuntime->push_on_stack(result);
}

void __is_neq__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->pop_and_deref_object_from_stack();
	FairyObject* lhs = pRuntime->pop_and_deref_object_from_stack();
	objectId result = pRuntime->allocate(*lhs != *rhs);
	pRuntime->push_on_stack(result);
}

void __less_than__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->pop_and_deref_object_from_stack();
	FairyObject* lhs = pRuntime->pop_and_deref_object_from_stack();
	objectId result = pRuntime->allocate(*lhs < *rhs);
	pRuntime->push_on_stack(result);
}

void __more_than__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->pop_and_deref_object_from_stack();
	FairyObject* lhs = pRuntime->pop_and_deref_object_from_stack();
	objectId result = pRuntime->allocate(*lhs > *rhs);
	pRuntime->push_on_stack(result);
}

void __less_eq_than__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->pop_and_deref_object_from_stack();
	FairyObject* lhs = pRuntime->pop_and_deref_object_from_stack();
	objectId result = pRuntime->allocate(*lhs <= *rhs);
	pRuntime->push_on_stack(result);
}

void __more_eq_than__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->pop_and_deref_object_from_stack();
	FairyObject* lhs = pRuntime->pop_and_deref_object_from_stack();
	objectId result = pRuntime->allocate(*lhs >= *rhs);
	pRuntime->push_on_stack(result);
}

void __minus_prefix__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->pop_and_deref_object_from_stack();
	objectId result = pRuntime->allocate(-rhs->asLong());
	pRuntime->push_on_stack(result);
}

void print_wrapper(Runtime* pRuntime, objectId context)
{
	objectId argId = pRuntime->soft_pop_and_deref_id_from_stack();
	stringId a = pRuntime->getObject(argId)->toStringId(pRuntime);
	printf("%s\n", pRuntime->getStringTable().getString(a));
	pRuntime->dec_ref(argId);
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
	pRuntime->push_on_stack(pRuntime->allocate(sid));
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
	objectId id = pRuntime->soft_pop_and_deref_id_from_stack();
	FairyObject* pObj = pRuntime->getObject(id);
	pRuntime->push_on_stack(pRuntime->allocate(pObj->toLong(pRuntime)));
	pRuntime->dec_ref(id);
}

void str_wrapper(Runtime* pRuntime, objectId context)
{
	objectId id = pRuntime->soft_pop_and_deref_id_from_stack();
	FairyObject* pObj = pRuntime->getObject(id);
	pRuntime->push_on_stack(pRuntime->allocate(pObj->toStringId(pRuntime)));
	pRuntime->dec_ref(id);
}

void bool_wrapper(Runtime* pRuntime, objectId context)
{
	objectId id = pRuntime->soft_pop_and_deref_id_from_stack();
	FairyObject* pObj = pRuntime->getObject(id);
	pRuntime->push_on_stack(pRuntime->allocate(pObj->toBool(pRuntime)));
	pRuntime->dec_ref(id);
}

void import_module(Runtime* pRuntime, objectId context)
{
	objectId id = pRuntime->soft_pop_and_deref_id_from_stack();
	FairyObject* pObj = pRuntime->getObject(id);
	std::string filename = pObj->toString(pRuntime);
	pRuntime->import_module(filename, -1);
	pRuntime->dec_ref(id);
}

// attrobj = getattr(obj, "key")
void getattr(Runtime* pRuntime, objectId context)
{
	FairyObject* pKey = pRuntime->pop_and_deref_object_from_stack();
	FairyObject* pTable = pRuntime->pop_and_deref_object_from_stack();
	pRuntime->push_on_stack(pTable->getattr(pRuntime, pKey->asString()));
}


// setattr(obj, "key", value)
void setattr(Runtime* pRuntime, objectId context)
{
	objectId value = pRuntime->pop_and_deref_id_from_stack();
	FairyObject* pKey = pRuntime->pop_and_deref_object_from_stack();
	FairyObject* pTable = pRuntime->pop_and_deref_object_from_stack();
	pTable->setattr(pRuntime, pKey->asString(), value);
}


// size = arr.size()
void array_size(Runtime* pRuntime, objectId context)
{
	FairyObject* thisObj = pRuntime->getObject(context);
	pRuntime->push_on_stack(thisObj->getattr(pRuntime, pRuntime->getStringTable().getStringId("__size")));
}

// size = arr.unroll()
void array_unroll(Runtime* pRuntime, objectId context)
{
	FairyObject* thisObj = pRuntime->getObject(context);
	objectId sizeObjId = thisObj->getattr(pRuntime, pRuntime->getStringTable().getStringId("__size"));
	FairyObject* sizeObj = pRuntime->getObject(sizeObjId);
	long long size = sizeObj->asLong();
	for (int i = 0; i < size; ++i)
	{
		pRuntime->push_on_stack(thisObj->getattr(pRuntime, pRuntime->getStringTable().getStringId(i)));
	}
}

void array_get(Runtime* pRuntime, objectId context)
{
	FairyObject* thisObj = pRuntime->getObject(context);
	objectId arg = pRuntime->pop_from_stack();
	FairyObject* indexObj = pRuntime->getObject(arg);
	long long index = indexObj->asLong();
	pRuntime->push_on_stack(thisObj->getattr(pRuntime, pRuntime->getStringTable().getStringId(index)));
}

void array_index(Runtime* pRuntime, objectId context)
{
	objectId argId = pRuntime->soft_pop_and_deref_id_from_stack();
	FairyObject* indexObj = pRuntime->getObject(argId);
	long long index = indexObj->asLong();
	pRuntime->push_on_stack(pRuntime->allocate(FairyReference{ context, pRuntime->getStringTable().getStringId(index) }));
	pRuntime->inc_ref(context);
	pRuntime->dec_ref(argId);
}

// obj = array(arg0, arg1, arg2, ..., argn);
void array(Runtime* pRuntime, objectId context)
{
	objectId newObjId = pRuntime->allocate(FairyObjectType::Array);
	pRuntime->inc_ref(newObjId);
	FairyObject* pObj = pRuntime->getObject(newObjId);
	assert(pRuntime->get_amount_of_objects_in_stack_frame() > 0);
	int amountOfArguments = pRuntime->get_amount_of_objects_in_stack_frame() - 1; //we do not want to pull "self" object
	int remainingObjects = amountOfArguments;
	while (remainingObjects)
	{

		objectId arg = pRuntime->soft_pop_from_stack();
		--remainingObjects;
		stringId sid = pRuntime->getStringTable().getStringId(remainingObjects); // index is equal to amount of objects-1
		pObj->setattr(pRuntime, sid, arg);
		pRuntime->dec_ref(arg);
	}
	objectId __sizeObjId = pRuntime->allocate((long long)amountOfArguments);
	pRuntime->getObject(newObjId)->setattr(pRuntime, pRuntime->getStringTable().getStringId("__size"), __sizeObjId);
	objectId sizeObjId = pRuntime->allocate(array_size);
	pRuntime->getObject(newObjId)->setattr(pRuntime, pRuntime->getStringTable().getStringId("size"), sizeObjId);
	objectId getObjId = pRuntime->allocate(array_get);
	pRuntime->getObject(newObjId)->setattr(pRuntime, pRuntime->getStringTable().getStringId("get"), getObjId);
	objectId index = pRuntime->allocate(array_index);
	pRuntime->getObject(newObjId)->setattr(pRuntime, pRuntime->getStringTable().getStringId("__index__"), index);
	//	pObj->setattr(pRuntime, pRuntime->getStringTable().getStringId("set"), pRuntime->allocate(array_set));
	objectId unrollObjId = pRuntime->allocate(array_unroll);
	pRuntime->getObject(newObjId)->setattr(pRuntime, pRuntime->getStringTable().getStringId("unroll"), unrollObjId);

	pRuntime->push_on_stack(newObjId);
	pRuntime->dec_ref(newObjId);
}

void object(Runtime* pRuntime, objectId context)
{
	pRuntime->push_on_stack(pRuntime->allocate(FairyObjectType::Object));
}