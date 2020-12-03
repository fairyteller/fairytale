#pragma once
#include "parsing.h"
#include "fairy_object.h"
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

class Runtime;



class BytecodeItem
{

};


struct Rules
{
	RangeBasedFilter spacesAndTabs;
	RangeBasedFilter digit;
	RangeBasedFilter brace;
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
		binaryOperatorPrecedence["@"] = 45;
		binaryOperatorPrecedence["("] = 48;
		binaryOperatorPrecedence["."] = 50;
	}
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

	std::unique_ptr<ASTNode> ParsePrefixUnaryOperator();
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

class StringTable
{
public:
	stringId getStringId(const std::string& str);
	stringId getStringId(long long number);
	stringId getStringId(const char* str);
	const char* getString(stringId id);
private:
	std::vector<std::string> stringTable;
	std::unordered_map<std::string, stringId> stringToIdHashtable;
};


class Runtime
{
public:
	Runtime(FairytaleCore* pCore);
	int execute(const std::vector<BytecodeItem>& bytecode);

	void set_unwinding(bool value) { unwinding = value; }
	bool is_unwinding() { return unwinding; }
	template<class T>
	objectId allocate(T value)
	{
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

	objectId allocate_empty()
	{
		if (freeIds.size())
		{
			objectId freeId = freeIds.top();
			allocatedObjects[freeId] = FairyObject();
			freeIds.pop();
			return freeId;
		}
		else
		{
			allocatedObjects.push_back(FairyObject());
			return allocatedObjects.size() - 1;
		}
	}

	objectId copy_object(objectId original)
	{
		objectId newCopyId = allocate_empty();
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

	void name_object(stringId sid, objectId od)
	{
		globalObjectTable[sid] = od;
		nameUsageStack.push(sid);
		inc_ref(od);
	}

	void name_object(objectId parentTable, stringId sid, objectId od)
	{
		getObject(parentTable)->setattr(this, sid, od);
	}

	void unbind_name(stringId strId)
	{
		dec_ref(globalObjectTable[strId]);
		globalObjectTable.erase(strId);
	}

	template<class T>
	objectId allocate_and_name(stringId strId, T value)
	{
		objectId id = allocate(value);
		name_object(strId, id);
		return id;
	}

	template<class T>
	objectId allocate_and_name(const char* s, T value)
	{
		stringId strId = m_stringTable.getStringId(s);
		return allocate_and_name(strId, value);
	}

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

	objectId get_existing_object_or_allocate(stringId strId)
	{
		auto iter = globalObjectTable.find(strId);
		if (iter == globalObjectTable.end())
		{
			objectId id = allocate_empty();
			name_object(strId, id);
			return id;
		}
		else
		{
			return iter->second;
		}
	}

	objectId get_existing_object_or_allocate(objectId parentTable, stringId strId)
	{
		if (parentTable == -1)
		{
			return get_existing_object_or_allocate(strId);
		}
		auto iter = getObject(parentTable)->get_table().find(strId);
		if (iter == getObject(parentTable)->get_table().end())
		{
			iter = globalObjectTable.find(strId);
			if (iter == globalObjectTable.end())
			{
				objectId id = allocate_empty();
				name_object(parentTable, strId, id);
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

	objectId register_function(const char* name, WrappedFunction fn)
	{
		objectId id = allocate_and_name(name, fn);
		return id;
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
		return getObject(pop_from_stack())->asLong();
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
		for (auto& obj : globalObjectTable)
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
			unbind_name(nameUsageStack.top());
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
private:
	struct StackFrame
	{
		size_t objectStackTop;
		size_t nameUsageStackTop;
	};
	StringTable m_stringTable;
	std::vector<FairyObject> allocatedObjects;
	std::stack<objectId> freeIds;
	FairyTable globalObjectTable;
	std::stack<objectId> interpreterStack;
	std::stack<stringId> nameUsageStack;
	std::stack<StackFrame> stackFrames;
	bool unwinding = false;
	objectId regId = -1;

	std::vector<std::unique_ptr<ASTNode>> loadedModuleASTs;
	FairytaleCore* m_core;
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
		pRuntime->push_on_stack(pRuntime->get_existing_object_or_allocate(context, id));
	}
};

class BinaryExprASTN : public ASTNode {
	std::string Op;
	std::unique_ptr<ASTNode> LHS, RHS;

public:
	BinaryExprASTN(const std::string& op, std::unique_ptr<ASTNode> LHS,
		std::unique_ptr<ASTNode> RHS)
		: Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
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

class PrefixOperatorExprASTN : public ASTNode {
	std::string Op;
	std::unique_ptr<ASTNode> RHS;

public:
	PrefixOperatorExprASTN(const std::string& op)
		: Op(op+"pref") {}
	void setRHS(std::unique_ptr<ASTNode> rhs)
	{
		RHS = std::move(rhs);
	}
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
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
		RHS->execute(pRuntime, oid);
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
	virtual void execute(Runtime* pRuntime, objectId context = -1)
	{
		Callee->execute(pRuntime, context);
		objectId calleeId = pRuntime->soft_pop_from_stack();
		for (auto& arg : Args)
		{
			arg->execute(pRuntime, context);
		}
		pRuntime->call(calleeId, context);
		pRuntime->dec_ref(calleeId);
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
		objectId global_scope = pRuntime->allocate_empty();
		pRuntime->inc_ref(global_scope);
		body->execute(pRuntime, global_scope);
		pRuntime->push_on_stack(global_scope);
		pRuntime->dec_ref(global_scope);

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
	virtual void call(Runtime* pRuntime, objectId self = -1)
	{
		objectId scope = pRuntime->copy_object(self);
		pRuntime->inc_ref(scope);
		for (auto arg = signature.rbegin(); arg != signature.rend(); ++arg)
		{
			stringId sid = pRuntime->getStringTable().getStringId(arg->c_str());
			objectId argid = pRuntime->get_existing_object_or_allocate(scope, sid);
			pRuntime->getObject(argid)->assign(pRuntime, pRuntime->getObject(pRuntime->stack_top()));
			pRuntime->pop_object_from_stack();
		}
		
		if (self != -1)
		{
			stringId sid = pRuntime->getStringTable().getStringId("self");
			objectId argid = pRuntime->get_existing_object_or_allocate(scope, sid);
			pRuntime->getObject(argid)->assign(pRuntime, pRuntime->getObject(self));
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
				pRuntime->save_to_register(pRuntime->stack_top());
			}
		}
		pRuntime->set_unwinding(true);
	}
};


