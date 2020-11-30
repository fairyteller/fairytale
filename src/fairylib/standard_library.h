#pragma once
#include "fairytale_core.h"
#include <cmath>
#include <stdio.h>
#include <string>

template<class Ret, class A0, class A1, typename F, F func>
void smart_wrapper(Runtime* pRuntime, objectId context)
{
	auto b = pRuntime->pop<A1>();
	auto a = pRuntime->pop<A0>();
	pRuntime->push_on_stack(pRuntime->allocate(func(a, b)));
}

template<class Ret, class A0, typename F, F func>
void smart_wrapper(Runtime* pRuntime, objectId context)
{
	auto a = pRuntime->pop<A0>();
	pRuntime->push_on_stack(pRuntime->allocate(func(a)));
}

template<class Ret, class A0, class A1, Ret(*func)(A0, A1)>
WrappedFunction wrap()
{
	return smart_wrapper<Ret, A0, A1, decltype(func), func>;
}

template<class Ret, class A0, Ret(*func)(A0)>
WrappedFunction wrap()
{
	return smart_wrapper<Ret, A0, decltype(func), func>;
}


long long sum(long long a, long long b)
{
	return a + b;
}

void sum_wrapper(Runtime* pRuntime, objectId context)
{
	FairyObject* b = pRuntime->getObject(pRuntime->pop_from_stack());
	FairyObject* a = pRuntime->getObject(pRuntime->pop_from_stack());
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
		objectId result = pRuntime->allocate( pRuntime->getStringTable().getStringId(summed.c_str()));
		pRuntime->push_on_stack(result);
	}
}

long long sub(long long a, long long b)
{
	return a - b;
}

void sub_wrapper(Runtime* pRuntime, objectId context)
{
	long long b = pRuntime->getObject(pRuntime->pop_from_stack())->asLong();
	long long a = pRuntime->getObject(pRuntime->pop_from_stack())->asLong();
	objectId result = pRuntime->allocate(sub(a, b));
	pRuntime->push_on_stack(result);
}

long long mul(long long a, long long b)
{
	return a * b;
}

void mul_wrapper(Runtime* pRuntime, objectId context)
{
	long long b = pRuntime->getObject(pRuntime->pop_from_stack())->asLong();
	long long a = pRuntime->getObject(pRuntime->pop_from_stack())->asLong();
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
	long long b = pRuntime->getObject(pRuntime->pop_from_stack())->asLong();
	long long a = pRuntime->getObject(pRuntime->pop_from_stack())->asLong();
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
	long long b = pRuntime->getObject(pRuntime->pop_from_stack())->asLong();
	long long a = pRuntime->getObject(pRuntime->pop_from_stack())->asLong();
	objectId result = pRuntime->allocate(pow(a, b));
	pRuntime->push_on_stack(result);
}

void assign(Runtime* pRuntime, objectId context)
{
	objectId rhsId = pRuntime->soft_pop_from_stack();
	FairyObject* rhs = pRuntime->getObject(rhsId);
	objectId lhsId = pRuntime->soft_pop_from_stack();
	FairyObject* lhs = pRuntime->getObject(lhsId);
	lhs->assign(pRuntime, rhs);
	pRuntime->push_on_stack(lhsId);
	pRuntime->dec_ref(rhsId);
	pRuntime->dec_ref(lhsId);
}

void sum_compound(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId lhsId = pRuntime->pop_from_stack();
	FairyObject* lhs = pRuntime->getObject(lhsId);
	*lhs += *rhs;
	pRuntime->push_on_stack(lhsId);
}

void sub_compound(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId lhsId = pRuntime->pop_from_stack();
	FairyObject* lhs = pRuntime->getObject(lhsId);
	*lhs -= *rhs;
	pRuntime->push_on_stack(lhsId);
}


void mul_compound(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId lhsId = pRuntime->pop_from_stack();
	FairyObject* lhs = pRuntime->getObject(lhsId);
	*lhs *= *rhs;
	pRuntime->push_on_stack(lhsId);
}


void div_compound(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId lhsId = pRuntime->pop_from_stack();
	FairyObject* lhs = pRuntime->getObject(lhsId);
	*lhs /= *rhs;
	pRuntime->push_on_stack(lhsId);
}


void mod_compound(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId lhsId = pRuntime->pop_from_stack();
	FairyObject* lhs = pRuntime->getObject(lhsId);
	*lhs %= *rhs;
	pRuntime->push_on_stack(lhsId);
}



void __is_eq__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	FairyObject* lhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId result = pRuntime->allocate( *lhs == *rhs);
	pRuntime->push_on_stack(result);
}

void __is_neq__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	FairyObject* lhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId result = pRuntime->allocate(*lhs != *rhs);
	pRuntime->push_on_stack(result);
}

void __less_than__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	FairyObject* lhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId result = pRuntime->allocate(*lhs < *rhs);
	pRuntime->push_on_stack(result);
}

void __more_than__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	FairyObject* lhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId result = pRuntime->allocate(*lhs > *rhs);
	pRuntime->push_on_stack(result);
}

void __less_eq_than__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	FairyObject* lhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId result = pRuntime->allocate(*lhs <= * rhs);
	pRuntime->push_on_stack(result);
}

void __more_eq_than__(Runtime* pRuntime, objectId context)
{
	FairyObject* rhs = pRuntime->getObject(pRuntime->pop_from_stack());
	FairyObject* lhs = pRuntime->getObject(pRuntime->pop_from_stack());
	objectId result = pRuntime->allocate(*lhs >= * rhs);
	pRuntime->push_on_stack(result);
}


void print_wrapper(Runtime* pRuntime, objectId context)
{
	stringId a = pRuntime->getObject(pRuntime->pop_from_stack())->toStringId(pRuntime);
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
	FairyObject* pObj = pRuntime->getObject(pRuntime->pop_from_stack());
	pRuntime->push_on_stack(pRuntime->allocate(pObj->toLong(pRuntime)));
}

void str_wrapper(Runtime* pRuntime, objectId context)
{
	FairyObject* pObj = pRuntime->getObject(pRuntime->pop_from_stack());
	pRuntime->push_on_stack(pRuntime->allocate(pObj->toStringId(pRuntime)));
}

void bool_wrapper(Runtime* pRuntime, objectId context)
{
	FairyObject* pObj = pRuntime->getObject(pRuntime->pop_from_stack());
	pRuntime->push_on_stack(pRuntime->allocate(pObj->toBool(pRuntime)));
}

void import_module(Runtime* pRuntime, objectId context)
{
	FairyObject* pObj = pRuntime->getObject(pRuntime->pop_from_stack());
	std::string filename = pObj->toString(pRuntime);
	pRuntime->import_module(filename, -1);
}

// attrobj = getattr(obj, "key")
void getattr(Runtime* pRuntime, objectId context)
{
	FairyObject* pKey = pRuntime->getObject(pRuntime->pop_from_stack());
	FairyObject* pTable = pRuntime->getObject(pRuntime->pop_from_stack());
	pRuntime->push_on_stack(pTable->getattr(pRuntime, pKey->asString()));
}


// setattr(obj, "key", value)
void setattr(Runtime* pRuntime, objectId context)
{
	objectId value = pRuntime->pop_from_stack();
	FairyObject* pKey = pRuntime->getObject(pRuntime->pop_from_stack());
	FairyObject* pTable = pRuntime->getObject(pRuntime->pop_from_stack());
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

// obj = array(arg0, arg1, arg2, ..., argn);
void array(Runtime* pRuntime, objectId context)
{
	objectId newObjId = pRuntime->allocate_empty();
	pRuntime->inc_ref(newObjId);
	FairyObject* pObj = pRuntime->getObject(newObjId);
	int amountOfArguments = pRuntime->get_amount_of_objects_in_stack_frame()-1; //we do not want to pull "self" object
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
//	pObj->setattr(pRuntime, pRuntime->getStringTable().getStringId("set"), pRuntime->allocate(array_set));
	objectId unrollObjId = pRuntime->allocate(array_unroll);
	pRuntime->getObject(newObjId)->setattr(pRuntime, pRuntime->getStringTable().getStringId("unroll"), unrollObjId);

	pRuntime->push_on_stack(newObjId);
	pRuntime->dec_ref(newObjId);
}
