#pragma once
#include "fairytale_core.h"
#include "runtime.h"
#include <cmath>
#include <stdio.h>
#include <string>

void sum_wrapper(Runtime* pRuntime, objectId context);

void sub_wrapper(Runtime* pRuntime, objectId context);

void mul_wrapper(Runtime* pRuntime, objectId context);

void div_wrapper(Runtime* pRuntime, objectId context);

void mod_wrapper(Runtime* pRuntime, objectId context);

void pow_wrapper(Runtime* pRuntime, objectId context);

void assign(Runtime* pRuntime, objectId context);

void sum_compound(Runtime* pRuntime, objectId context);

void sub_compound(Runtime* pRuntime, objectId context);

void mul_compound(Runtime* pRuntime, objectId context);

void div_compound(Runtime* pRuntime, objectId context);

void mod_compound(Runtime* pRuntime, objectId context);

void __is_eq__(Runtime* pRuntime, objectId context);

void __is_neq__(Runtime* pRuntime, objectId context);

void __less_than__(Runtime* pRuntime, objectId context);

void __more_than__(Runtime* pRuntime, objectId context);

void __less_eq_than__(Runtime* pRuntime, objectId context);

void __more_eq_than__(Runtime* pRuntime, objectId context);

void __minus_prefix__(Runtime* pRuntime, objectId context);

void print_wrapper(Runtime* pRuntime, objectId context);

char* input();

void input_wrapper(Runtime* pRuntime, objectId context);

void trace(Runtime* pRuntime, objectId context);

void trace_names(Runtime* pRuntime, objectId context);

void print_stack(Runtime* pRuntime, objectId context);

void int_wrapper(Runtime* pRuntime, objectId context);

void str_wrapper(Runtime* pRuntime, objectId context);

void bool_wrapper(Runtime* pRuntime, objectId context);

void import_module(Runtime* pRuntime, objectId context);

// attrobj = getattr(obj, "key")
void getattr(Runtime* pRuntime, objectId context);

// setattr(obj, "key", value)
void setattr(Runtime* pRuntime, objectId context);


// attrobj = getattr_direct(obj, "key")
void getattr_direct(Runtime* pRuntime, objectId context);

// setattr_direct(obj, "key", value)
void setattr_direct(Runtime* pRuntime, objectId context);


// size = arr.size()
void array_size(Runtime* pRuntime, objectId context);

// size = arr.unroll()
void array_unroll(Runtime* pRuntime, objectId context);

void array_get(Runtime* pRuntime, objectId context);

void array_index(Runtime* pRuntime, objectId context);

// obj = array(arg0, arg1, arg2, ..., argn);
void array(Runtime* pRuntime, objectId context);

void object(Runtime* pRuntime, objectId context);