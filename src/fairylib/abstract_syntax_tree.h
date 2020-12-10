#pragma once
#include "runtime.h"
#include "fairy_object.h"
#include <memory>

class ASTNode
{
public:
	virtual ~ASTNode() {};
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{

	}
};

class NumberASTN : public ASTNode
{
	long long m_value;
public:
	NumberASTN(long long value) : m_value(value) {}
	NumberASTN(const std::string& str)
	{
		m_value = std::stoll(str, 0);
	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		pRuntime->allocate_on_stack(m_value);
	}
};

class BooleanASTN : public ASTNode
{
	bool m_value;
public:
	BooleanASTN(bool value) : m_value(value) {}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		pRuntime->allocate_on_stack(m_value);
	}
};

class StringASTN : public ASTNode {
	std::string stringValue;

public:
	StringASTN(const std::string& content) : stringValue(content) {}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		stringId strId = pRuntime->getStringTable().getStringId(stringValue.c_str());
		pRuntime->allocate_on_stack<stringId>(strId);
	}
};

class VariableASTN : public ASTNode {
	std::string Name;

public:
	VariableASTN(const std::string& Name) : Name(Name) {}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		stringId id = pRuntime->getStringTable().getStringId(Name);
		pRuntime->allocate_on_stack(FairyReference{ context, id });
		// TODO: fix this manual ref increment, it can cause memory leak
		pRuntime->inc_ref(context);
	}
};

class BinaryExprASTN : public ASTNode {
	std::string Op;
	std::unique_ptr<ASTNode> LHS, RHS;
public:
	BinaryExprASTN(const std::string& op, std::unique_ptr<ASTNode> LHS,
		std::unique_ptr<ASTNode> RHS)
		: Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	virtual void execute(Runtime* pRuntime, objectId context)
	{
		if (LHS.get())
			LHS->execute(pRuntime, context);
		if (RHS.get())
			RHS->execute(pRuntime, context);
		stringId sid = pRuntime->getStringTable().getStringId(Op);
		objectId oid = pRuntime->get_existing_object_or_allocate(context, sid);
		pRuntime->call(oid);
	}
};

class AccessAttribASTN : public ASTNode {
	std::string Op;
	std::unique_ptr<ASTNode> LHS, RHS;

public:
	AccessAttribASTN(const std::string& op, std::unique_ptr<ASTNode> LHS,
		std::unique_ptr<ASTNode> RHS)
		: Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		LHS->execute(pRuntime, context);
		ObjectRef lhs = pRuntime->safe_pop_and_dereference();
		RHS->execute(pRuntime, lhs.id());
	}
};

class CallExprASTN : public ASTNode {
	std::unique_ptr<ASTNode> Callee;
	std::vector<std::unique_ptr<ASTNode>> Args;

public:
	CallExprASTN(std::unique_ptr<ASTNode> Callee,
		std::vector<std::unique_ptr<ASTNode>> Args)
		: Callee(std::move(Callee)), Args(std::move(Args)) {}
	virtual void execute(Runtime* pRuntime, objectId context)
	{
		Callee->execute(pRuntime, context);
		ObjectRef callee = pRuntime->safe_pop();

		for (auto& arg : Args)
		{
			arg->execute(pRuntime, context);
		}
		if (callee->getType() == FairyObjectType::Reference)
		{
			ObjectRef dereferencedCallee = pRuntime->safe_dereference(callee);
			pRuntime->call(dereferencedCallee.id(), callee->asReference().ownerTable);
		}
		else
		{
			pRuntime->call(callee.id(), context);
		}
	}
};


class IndexExprASTN : public ASTNode {
	std::unique_ptr<ASTNode> Collection;
	std::vector<std::unique_ptr<ASTNode>> Args;

public:
	IndexExprASTN(std::unique_ptr<ASTNode> Collection,
		std::vector<std::unique_ptr<ASTNode>> Args)
		: Collection(std::move(Collection)), Args(std::move(Args)) {}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		Collection->execute(pRuntime, context);
		ObjectRef collection = pRuntime->safe_pop_and_dereference();
		for (auto& arg : Args)
		{
			arg->execute(pRuntime, context);
		}
		stringId indexMethodSID = pRuntime->getStringTable().getStringId("__index__");
		objectId indexMethod = collection->getattr(pRuntime, indexMethodSID);
		pRuntime->call(indexMethod, collection.id());
	}
};


class RootASTN : public ASTNode {
	std::unique_ptr<ASTNode> body;

public:
	RootASTN(std::unique_ptr<ASTNode>&& node) {
		body = std::move(node);
	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		ObjectRef module_scope(pRuntime, pRuntime->get_global_scope());
		body->execute(pRuntime, module_scope.id());
		pRuntime->push_on_stack(module_scope.id());
	}
};

class SequenceASTN : public ASTNode {
	std::vector<std::unique_ptr<ASTNode>> sequence;

public:
	SequenceASTN() {}
	void push_back(std::unique_ptr<ASTNode>&& node)
	{
		sequence.emplace_back(std::move(node));
	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		pRuntime->push_frame();
		for (auto& arg : sequence)
		{
			if (pRuntime->is_stack_unwinding())
				break;
			pRuntime->clear_stack();
			arg->execute(pRuntime, context);
		}
		pRuntime->pop_frame();
	}
};

class ConditionASTN : public ASTNode {
	std::unique_ptr<ASTNode> condition;
	std::unique_ptr<ASTNode> trueBlock;
	std::unique_ptr<ASTNode> falseBlock;

public:
	ConditionASTN() {}
	void setCondition(std::unique_ptr<ASTNode>&& node)
	{
		condition = std::move(node);
	}
	void setTrueBlock(std::unique_ptr<ASTNode>&& node)
	{
		trueBlock = std::move(node);
	}
	void setFalseBlock(std::unique_ptr<ASTNode>&& node)
	{
		falseBlock = std::move(node);
	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		condition->execute(pRuntime, context);
		ObjectRef conditionResult = pRuntime->safe_pop_and_dereference();
		if (conditionResult->asBool() == true)
		{
			trueBlock->execute(pRuntime, context);
		}
		else
		{
			if (falseBlock.get())
				falseBlock->execute(pRuntime, context);
		}
	}
};


class TryCatchFinallyASTN : public ASTNode {
	std::unique_ptr<ASTNode> tryBlock;
	std::unique_ptr<ASTNode> catchBlock;
	std::unique_ptr<ASTNode> finallyBlock;
	std::string exceptionVariableName;
public:
	TryCatchFinallyASTN() {}
	void setTryBlock(std::unique_ptr<ASTNode>&& node)
	{
		tryBlock = std::move(node);
	}
	void setCatchBlock(std::unique_ptr<ASTNode>&& node)
	{
		catchBlock = std::move(node);
	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		tryBlock->execute(pRuntime, context);
		if (pRuntime->is_exception_raised() && catchBlock.get())
		{
			pRuntime->set_exception_as_handled();
			catchBlock->execute(pRuntime, context);
		}
		if (finallyBlock.get())
			finallyBlock->execute(pRuntime, context);
	}
};


class WhileASTN : public ASTNode {
	std::unique_ptr<ASTNode> condition;
	std::unique_ptr<ASTNode> innerBlock;

public:
	WhileASTN() {}
	void setCondition(std::unique_ptr<ASTNode>&& node)
	{
		condition = std::move(node);
	}
	void setInnerBlock(std::unique_ptr<ASTNode>&& node)
	{
		innerBlock = std::move(node);
	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		condition->execute(pRuntime, context);
		ObjectRef conditionResult = pRuntime->safe_pop_and_dereference();
		while (conditionResult->asBool() == true)
		{
			innerBlock->execute(pRuntime, context);
			condition->execute(pRuntime, context);
			if (pRuntime->is_stack_unwinding())
			{
				break;
			}
			conditionResult = pRuntime->safe_pop_and_dereference();
		}
	}
};

class FuncASTN : public ASTNode {
	std::vector<std::string> signature;
	std::unique_ptr<ASTNode> functionBody;

public:
	FuncASTN() {}
	void setSignature(std::vector<std::string>&& _signature)
	{
		signature = _signature;
	}
	void setFunctionBody(std::unique_ptr<ASTNode>&& node)
	{
		functionBody = std::move(node);
	}
	virtual void call(Runtime* pRuntime, objectId self)
	{
		ObjectRef scope(pRuntime, pRuntime->copy_object(pRuntime->get_global_scope()));
		for (auto arg = signature.rbegin(); arg != signature.rend(); ++arg)
		{
			stringId sid = pRuntime->getStringTable().getStringId(arg->c_str());
			pRuntime->assign_name_to_obj(scope.id(), sid, pRuntime->soft_pop_and_deref_id_from_stack());
		}

		if (self != -1)
		{
			stringId sid = pRuntime->getStringTable().getStringId("self");
			pRuntime->assign_name_to_obj(scope.id(), sid, self);
		}

		pRuntime->push_frame();
		functionBody->execute(pRuntime, scope.id());
		pRuntime->pop_frame();
		if (pRuntime->is_exception_raised())
			return;
		pRuntime->set_stack_unwinding(false);
		// TODO: refactor this unsafe passing, release of an object or memory leak may occur
		objectId id = pRuntime->copy_from_register();
		if (id != -1)
		{
			pRuntime->push_on_stack(id);
			pRuntime->load_from_register();
		}

	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		pRuntime->allocate_on_stack(this);
	}
};

class ReturnASTN : public ASTNode {
	std::unique_ptr<ASTNode> expression;

public:
	ReturnASTN() {}
	void setExpression(std::unique_ptr<ASTNode>&& node)
	{
		expression = std::move(node);
	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		if (expression.get())
		{
			expression->execute(pRuntime, context);
			if (!pRuntime->is_frame_stack_empty())
			{
				ObjectRef stackTop = pRuntime->safe_pop_and_dereference();
				pRuntime->save_to_register(stackTop.id());
			}
		}
		pRuntime->set_stack_unwinding(true);
	}
};

class ThrowASTN : public ASTNode {
	std::unique_ptr<ASTNode> expression;

public:
	ThrowASTN() {}
	void setExpression(std::unique_ptr<ASTNode>&& node)
	{
		expression = std::move(node);
	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		if (expression.get())
		{
			expression->execute(pRuntime, context);
			if (!pRuntime->is_frame_stack_empty())
			{
				ObjectRef stackTop = pRuntime->safe_pop_and_dereference();
				pRuntime->throw_exception(stackTop.id());
			}
		}

	}
};

