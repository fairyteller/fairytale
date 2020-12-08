#pragma once
#include "parsing.h"
#include "fairy_object.h"
#include "runtime.h"
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

struct Rules
{
	RangeBasedFilter spacesAndTabs;
	RangeBasedFilter digit;
	RangeBasedFilter brace;
	RangeBasedFilter squareBrace;
	RangeBasedFilter operatorSymbols;
	RangeBasedFilter openParenthesis;
	RangeBasedFilter englishLetters;
	RangeBasedFilter parenthesis;
	RangeBasedFilter validIdentifierFirstSymbol;
	RangeBasedFilter validIdentifierSymbol;
	RangeBasedFilter expressionEnd;
	RangeBasedFilter quote;
	RangeBasedFilter stringContentSymbol;
	RangeBasedFilter numberSign;
	RangeBasedFilter commentContentSymbol;
	RangeBasedFilter commentEnd;

	TokenRules numberRule;
	TokenRules braceRule;
	TokenRules operatorRule;
	TokenRules identifierRule;
	TokenRules parenthesisRule;
	TokenRules expressionEndRule;
	TokenRules stringLiteralRule;
	TokenRules commentRule;
	TokenRules squareBracetRule;

	std::map<std::string, int> binaryOperatorPrecedence;

	int GetOperatorPrecedence(const std::string& requestedOperator) {
		int TokPrec = binaryOperatorPrecedence[requestedOperator];
		if (TokPrec <= 0) return -1;
		return TokPrec;
	}

	static Token createNumberToken(const std::string& input, size_t startIndex, size_t endIndex)
	{
		Token token;
		token.type = TokenType::Number;
		token.content = input.substr(startIndex, endIndex - startIndex);
		return token;
	}

	static Token createParenthesisToken(const std::string& input, size_t startIndex, size_t endIndex)
	{
		Token token;
		if (input[startIndex] == '(')
			token.type = TokenType::OpenParenthesis;
		if (input[startIndex] == ')')
			token.type = TokenType::ClosedParenthesis;
		if (input[startIndex] == ',')
			token.type = TokenType::Comma;
		token.content = input[startIndex];
		return token;
	}

	static Token createBraceToken(const std::string& input, size_t startIndex, size_t endIndex)
	{
		Token token;
		if (input[startIndex] == '{')
			token.type = TokenType::OpenBrace;
		if (input[startIndex] == '}')
			token.type = TokenType::ClosedBrace;
		token.content = input[startIndex];
		return token;
	}

	static Token createSquareBraceToken(const std::string& input, size_t startIndex, size_t endIndex)
	{
		Token token;
		if (input[startIndex] == '[')
			token.type = TokenType::OpenSquareBrace;
		if (input[startIndex] == ']')
			token.type = TokenType::ClosedSquareBrace;
		token.content = input[startIndex];
		return token;
	}
	static Token createOperatorToken(const std::string& input, size_t startIndex, size_t endIndex)
	{
		Token token;
		token.type = TokenType::BinaryOperator;
		token.content = input.substr(startIndex, endIndex - startIndex);
		return token;
	}

	static Token createIdentifierToken(const std::string& input, size_t startIndex, size_t endIndex)
	{
		Token token;
		token.content = input.substr(startIndex, endIndex - startIndex);
		auto assumedKeyword = input.substr(startIndex, endIndex - startIndex);
		if (assumedKeyword == "true" || assumedKeyword == "false")
		{
			token.type = TokenType::BooleanLiteral;
		}
		else if (assumedKeyword == "if")
		{
			token.type = TokenType::If;
		}
		else if (assumedKeyword == "else")
		{
			token.type = TokenType::Else;
		}
		else if (assumedKeyword == "while")
		{
			token.type = TokenType::While;
		}
		else if (assumedKeyword == "func")
		{
			token.type = TokenType::Func;
		}
		else if (assumedKeyword == "return")
		{
			token.type = TokenType::Return;
		}
//		else if (assumedKeyword == "import")
//		{
//			token.type = TokenType::Import;
//		}
		
		else
		{
			token.type = TokenType::Identifier;
		}
		return token;
	}

	static Token createStringLiteralToken(const std::string& input, size_t startIndex, size_t endIndex)
	{
		Token token;
		token.type = TokenType::String;
		token.content = input.substr(startIndex+1, endIndex - startIndex - 2);
		return token;

	}

	static Token createExpressionEndToken(const std::string& input, size_t startIndex, size_t endIndex)
	{
		Token token;
		token.type = TokenType::ExpressionEnd;
		token.content = input.substr(startIndex, endIndex - startIndex);
		return token;
	}

	size_t skipIndents(const std::string& input, size_t startIndex)
	{
		return readWhileFiltered(input, startIndex, spacesAndTabs);
	}

	Rules()
	{
		spacesAndTabs.set('\t').unite(' ');
		digit.set({ '0' , '9' });
		operatorSymbols.set('+').unite('-').unite('*').unite('/').unite('=').unite('>').unite('<').unite('&').unite('|').unite('!').unite('%').unite('.').unite('@');
		parenthesis.set('(').unite(')').unite(',');
		brace.set('{').unite('}');
		squareBrace.set('[').unite(']');
		englishLetters.set({ 'A','Z' }).unite({ 'a', 'z' });
		validIdentifierFirstSymbol.set({ 'A','Z' }).unite({ 'a', 'z' }).unite('_');
		validIdentifierSymbol.set({ 'A','Z' }).unite({ 'a', 'z' }).unite('_').unite({ '0','9' });
		expressionEnd.set('\n').unite(';').unite('\0');
		commentEnd.set('\n').unite('\0');
		quote.set('\"');
		numberSign.set('#');
		stringContentSymbol.set({ 1, '\"'-1 }).unite({'\"'+1, 126});
		commentContentSymbol.set({ 1, '\n' - 1 }).unite({ '\n' + 1, 126 });
		
		numberRule.setFactory(createNumberToken) << SymbolSequence{1, UINT32_MAX, digit };
		operatorRule.setFactory(createOperatorToken) << SymbolSequence{ 1, 2, operatorSymbols };
		identifierRule.setFactory(createIdentifierToken) << SymbolSequence{ 1, 1, validIdentifierFirstSymbol } << SymbolSequence{ 0, UINT32_MAX, validIdentifierSymbol };
		parenthesisRule.setFactory(createParenthesisToken) << SymbolSequence{ 1, 1, parenthesis };
		braceRule.setFactory(createBraceToken) << SymbolSequence{ 1, 1, brace };
		squareBracetRule.setFactory(createSquareBraceToken) << SymbolSequence{ 1, 1, squareBrace };
		expressionEndRule.setFactory(createExpressionEndToken) << SymbolSequence{ 1, 1, expressionEnd };
		stringLiteralRule.setFactory(createStringLiteralToken) << SymbolSequence{ 1, 1, quote} << SymbolSequence{0, UINT32_MAX, stringContentSymbol } << SymbolSequence{1, 1, quote};
		commentRule.setFactory(nullptr) << SymbolSequence{ 1, 1, numberSign } << SymbolSequence{ 0, UINT32_MAX, commentContentSymbol } << SymbolSequence{ 1, 1, commentEnd };

		binaryOperatorPrecedence["="] = 5;
		binaryOperatorPrecedence["+="] = 5;
		binaryOperatorPrecedence["-="] = 5;
		binaryOperatorPrecedence["*="] = 5;
		binaryOperatorPrecedence["%="] = 5;
		binaryOperatorPrecedence["/="] = 5;
		binaryOperatorPrecedence["=="] = 9;
		binaryOperatorPrecedence["!="] = 9;
		binaryOperatorPrecedence[">="] = 10;
		binaryOperatorPrecedence["<="] = 10;
		binaryOperatorPrecedence[">"] = 10;
		binaryOperatorPrecedence["<"] = 10;
		binaryOperatorPrecedence["+"] = 20;
		binaryOperatorPrecedence["-"] = 20;
		binaryOperatorPrecedence["*"] = 40;
		binaryOperatorPrecedence["%"] = 40;
		binaryOperatorPrecedence["/"] = 40;
		binaryOperatorPrecedence["-prefix"] = 45;
		binaryOperatorPrecedence["("] = 48;
		binaryOperatorPrecedence["["] = 48;
		binaryOperatorPrecedence["."] = 50;
	}
};

class BytecodeItem {

};

class ASTNode;
class Lexer
{
public:
	std::vector<BytecodeItem> getBytecode(const std::string& input, Runtime* pRuntime);
	void tokenize(const std::string& input);
	std::vector<Token> convertToPostfix(const std::vector<Token>& tokens);
	std::unique_ptr<ASTNode> buildAST();
	std::unique_ptr<ASTNode> getAST(const std::string& input);
	std::vector<BytecodeItem> translate(const std::vector<Token>& tokens);


	std::unique_ptr<ASTNode> ParseFileRoot();
	std::unique_ptr<ASTNode> ParseStatement();
	std::unique_ptr<ASTNode> ParseBlock();
	std::unique_ptr<ASTNode> ParseIf();
	std::unique_ptr<ASTNode> ParseWhile();
	std::unique_ptr<ASTNode> ParseReturn();
	std::unique_ptr<ASTNode> ParseStringExpr();

	std::vector < std::string> ParseFunctionSignature();

	std::unique_ptr<ASTNode> ParseFunctionDeclaration();
	std::unique_ptr<ASTNode> ParseNumberExpr();

	std::unique_ptr<ASTNode> ParseParenExpr();

	
	std::unique_ptr<ASTNode> ParseBooleanExpr();
	std::unique_ptr<ASTNode> ParseIdentifierExpr();

	std::unique_ptr<ASTNode> ParsePrimary();

	std::unique_ptr<ASTNode> ParseExpression();

	std::unique_ptr<ASTNode> ParseBinaryOperatorRHS(int ExprPrec,
		std::unique_ptr<ASTNode> LHS);
private:
	Rules rules;
	std::vector<Token> tokens;
	size_t currentToken;
};

class FairytaleCore
{
public:
	FairytaleCore() : m_runtime(this) {}
	virtual int interpret(const char* line);
	int interpret(const std::string& input);
	virtual int interpret_file(const char* filename);

	std::unique_ptr<ASTNode> loadAST(const std::string& input);
private:
	Lexer m_lexer;
	Runtime m_runtime;
};

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
		pRuntime->put_literal_on_stack(m_value);
	}
};

class BooleanASTN : public ASTNode
{
	bool m_value;
public:
	BooleanASTN(bool value) : m_value(value) {}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		pRuntime->put_literal_on_stack(m_value);
	}
};

class StringASTN : public ASTNode {
	std::string stringValue;

public:
	StringASTN(const std::string& content) : stringValue(content) {}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		stringId strId = pRuntime->getStringTable().getStringId(stringValue.c_str());
		pRuntime->put_literal_on_stack<stringId>(strId);
	}
};

class VariableASTN : public ASTNode {
	std::string Name;

public:
	VariableASTN(const std::string& Name) : Name(Name) {}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		stringId id = pRuntime->getStringTable().getStringId(Name);
		pRuntime->push_on_stack(pRuntime->allocate(FairyReference{ context, id }));
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
		objectId oid = pRuntime->soft_pop_from_stack();
		objectId dereferencedId = pRuntime->dereference(oid);
		RHS->execute(pRuntime, dereferencedId);
		pRuntime->dec_ref(oid);
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
		objectId calleeId = pRuntime->soft_pop_from_stack();
		objectId derefCalleeId = pRuntime->dereference(calleeId);

		for (auto& arg : Args)
		{
			arg->execute(pRuntime, context);
		}
		FairyObject* pObj = pRuntime->getObject(calleeId);
		if (pObj->getType() == FairyObjectType::Reference)
		{
			pRuntime->call(derefCalleeId, pObj->asReference().ownerTable);
		}
		else
		{
			pRuntime->call(derefCalleeId, context);
		}
		pRuntime->dec_ref(calleeId);
		pRuntime->dec_ref(derefCalleeId);
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
		objectId collectionId = pRuntime->soft_pop_and_deref_id_from_stack();
		for (auto& arg : Args)
		{
			arg->execute(pRuntime, context);
		}
		stringId indexMethodSID = pRuntime->getStringTable().getStringId("__index__");
		objectId indexMethod = pRuntime->getObject(collectionId)->getattr(pRuntime, indexMethodSID);
		pRuntime->call(indexMethod, collectionId);
		pRuntime->dec_ref(collectionId);
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
		objectId module_scope = pRuntime->getGlobalScope();
		pRuntime->inc_ref(module_scope);
		body->execute(pRuntime, module_scope);
		pRuntime->push_on_stack(module_scope);
		pRuntime->dec_ref(module_scope);
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
			if (pRuntime->is_unwinding())
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
		FairyObject* pObj = pRuntime->pop_object_from_stack();
		if (pObj->asBool() == true)
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
		FairyObject* pObj = pRuntime->pop_object_from_stack();
		while (pObj->asBool() == true)
		{
			innerBlock->execute(pRuntime, context);
			condition->execute(pRuntime, context);
			if (pRuntime->is_unwinding())
			{
				break;
			}
			pObj = pRuntime->pop_object_from_stack();
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
		objectId scope = pRuntime->copy_object(pRuntime->getGlobalScope());
		pRuntime->inc_ref(scope);
		for (auto arg = signature.rbegin(); arg != signature.rend(); ++arg)
		{
			stringId sid = pRuntime->getStringTable().getStringId(arg->c_str());
			pRuntime->assign_name_to_obj(scope, sid, pRuntime->soft_pop_and_deref_id_from_stack());
		}
		
		if (self != -1)
		{
			stringId sid = pRuntime->getStringTable().getStringId("self");
			pRuntime->assign_name_to_obj(scope, sid, self);
		}

		pRuntime->push_frame();
		functionBody->execute(pRuntime, scope);
		pRuntime->dec_ref(scope);
		pRuntime->pop_frame();
		pRuntime->set_unwinding(false);
		objectId id = pRuntime->copy_from_register();
		if (id != -1)
		{
			pRuntime->push_on_stack(id);
			pRuntime->load_from_register();
		}

	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		pRuntime->put_literal_on_stack(this);
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
				pRuntime->save_to_register(pRuntime->dereference(pRuntime->stack_top()));
			}
		}
		pRuntime->set_unwinding(true);
	}
};


