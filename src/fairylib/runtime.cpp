#include "runtime.h"
#include "standard_library.h"
#include "abstract_syntax_tree.h"
#include "fairytale_core.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Runtime::Runtime(FairytaleCore* pCore)
	: m_core(pCore)
	, globalScopeObject(-1)
	, direct_memory_usage_semaphore(0)
{
	noneObject = allocate(FairyObjectType::None);
	inc_ref(noneObject);
	globalScopeObject = allocate(FairyObjectType::Module);
	inc_ref(globalScopeObject);
	assign_name_to_obj(globalScopeObject, "None", noneObject);
	register_global_function("pow", pow_wrapper);
	register_global_function("+", sum_wrapper);
	register_global_function("-", sub_wrapper);
	register_global_function("*", mul_wrapper);
	register_global_function("/", div_wrapper);
	register_global_function("%", mod_wrapper);
	register_global_function("+=", sum_compound);
	register_global_function("-=", sub_compound);
	register_global_function("*=", mul_compound);
	register_global_function("/=", div_compound);
	register_global_function("%=", mod_compound);
	register_global_function("==", __is_eq__);
	register_global_function("!=", __is_neq__);
	register_global_function("<", __less_than__);
	register_global_function(">", __more_than__);
	register_global_function("<=", __less_eq_than__);
	register_global_function(">=", __more_eq_than__);
	register_global_function("-prefix", __minus_prefix__);
	register_global_function("=", assign);
	register_global_function("print", print_wrapper);
	register_global_function("trace", trace);
	register_global_function("trace_names", ::trace_names);
	register_global_function("print_stack", print_stack);
	register_global_function("input", input_wrapper);
	register_global_function("int", int_wrapper);
	register_global_function("str", str_wrapper);
	register_global_function("bool", bool_wrapper);
	register_global_function("import_module", ::import_module);
	register_global_function("getattr", ::getattr);
	register_global_function("setattr", ::setattr);
	register_global_function("getattr", getattr_direct);
	register_global_function("setattr", setattr_direct);
	register_global_function("array", array);
	register_global_function("object", object);
}

std::unique_ptr<ASTNode> Runtime::load_module(const std::string& filename)
{
	std::ifstream file;
	file.open(filename);
	std::string input;

	std::stringstream sstr;

	while (file >> sstr.rdbuf());

	input = sstr.str();

	return std::move(m_core->loadAST(filename, input));
}

int Runtime::execute_ast(std::unique_ptr<ASTNode>&& ast)
{
	loadedModuleASTs.emplace_back(std::move(ast));
	loadedModuleASTs.back()->execute(this);
	if (isExceptionRaised)
		return 1;
	else
		return 0;
}

void Runtime::call_method(objectId id, stringId methodId)
{
	ObjectRef ref(this, id);
	ObjectRef dereferenced_obj = safe_dereference(ref);
	objectId callee = getattr(dereferenced_obj.id(), methodId);
	call(callee, dereferenced_obj.id());

}

void Runtime::call(objectId id, objectId context)
{
	FairyObject* pObj = getObject(id);
	if (pObj->getType() == FairyObjectType::WrappedFunction)
	{
		pObj->asWrappedFunction()(this, context);
	}
	else if (pObj->getType() == FairyObjectType::FairyFunction)
	{
		pObj->asFairyFunction()->call(this, context);
	}
	else
		throw_runtime_error("Object is not callable");
}

std::string Runtime::get_last_stack_trace()
{
	std::stringstream result;
	while (!savedStackTrace.empty())
	{
		std::string functionName = getStringTable().getString(savedStackTrace.top().functionName);
		result << "at " << functionName << " (" << *(savedStackTrace.top().filename) << ", line " <<  savedStackTrace.top().lineNo << " )\n";
		savedStackTrace.pop();
	}
	return result.str();
}