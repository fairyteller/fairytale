#include "fairytale_core.h"
#include "standard_library.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

bool g_trace = false;

stringId StringTable::getStringId(const std::string& str)
{
	auto strIter = stringToIdHashtable.find(str);
	if (strIter != stringToIdHashtable.end())
		return strIter->second;
	else
	{
		stringTable.push_back(str);
		stringId id = stringTable.size() - 1;
		stringToIdHashtable[str] = id;
		return id;
	}
}

stringId StringTable::getStringId(const char* str)
{
	std::string s(str);
	stringId id = getStringId(s);
	return id;
}

stringId StringTable::getStringId(long long number)
{
	stringId id = getStringId(std::to_string(number));
	return id;
}


const char* StringTable::getString(stringId id)
{
	return stringTable[id].c_str();
}


void FairyObject::assign(Runtime* pRuntime, FairyObject* rhs)
{
	m_type = rhs->m_type;
	m_value = rhs->m_value;
	table = rhs->table;
	for (auto& attrib : table)
	{
		pRuntime->inc_ref(attrib.second);
	}
}

void FairyObject::release(Runtime* pRuntime)
{
	for (auto& attrib : table)
	{
		pRuntime->dec_ref(attrib.second);
	}
	table.clear();
	if (m_type == FairyObjectType::Reference)
	{
		pRuntime->dec_ref(m_value.asReference.ownerTable);
	}
}

long long FairyObject::toLong(Runtime* pRuntime)
{
	if (m_type == FairyObjectType::Int)
		return m_value.asLong;
	if (m_type == FairyObjectType::Bool)
		return m_value.asBool;
	if (m_type == FairyObjectType::String)
		return atoll(pRuntime->getStringTable().getString(m_value.asString));
	assert(!"Type is not covnertible into int");
}

stringId FairyObject::toStringId(Runtime* pRuntime)
{
	if (m_type == FairyObjectType::String)
		return m_value.asString;
	else
		return pRuntime->getStringTable().getStringId(toString(pRuntime).c_str());
}


std::string FairyObject::toString(Runtime* pRuntime)
{
	if (m_type == FairyObjectType::Int)
		return std::to_string(m_value.asLong);
	if (m_type == FairyObjectType::Bool)
		return m_value.asBool ? "true" : "false";
	if (m_type == FairyObjectType::String)
		return pRuntime->getStringTable().getString(m_value.asString);
	assert(!"Type is not covnertible into str");
}

bool FairyObject::toBool(Runtime* pRuntime)
{
	if (m_type == FairyObjectType::Int)
		return (bool)m_value.asLong;
	if (m_type == FairyObjectType::Bool)
		return m_value.asBool;
	if (m_type == FairyObjectType::String)
		return pRuntime->getStringTable().getStringId("") != m_value.asString;
	assert(!"Type is not covnertible into int");
}

//TODO: implement bytecode caching
void Lexer::tokenize(const std::string& input)
{
	tokens.clear();
	currentToken = 0;
	size_t currIndex = 0;
	size_t nextIndex = 0;
	while (input[currIndex] != 0)
	{
		currIndex = rules.skipIndents(input, currIndex);
		nextIndex = rules.operatorRule.checkIfToken(input.c_str(), currIndex);
		if (nextIndex != currIndex)
		{
			tokens.push_back(rules.operatorRule.build(input, currIndex, nextIndex));
			currIndex = nextIndex;
			continue;
		}
		nextIndex = rules.numberRule.checkIfToken(input.c_str(), currIndex);
		if (nextIndex != currIndex)
		{
			tokens.push_back(rules.numberRule.build(input, currIndex, nextIndex));
			currIndex = nextIndex;
			continue;
		}
		nextIndex = rules.identifierRule.checkIfToken(input.c_str(), currIndex);
		if (nextIndex != currIndex)
		{
			tokens.push_back(rules.identifierRule.build(input, currIndex, nextIndex));
			currIndex = nextIndex;
			continue;
		}
		nextIndex = rules.squareBracetRule.checkIfToken(input.c_str(), currIndex);
		if (nextIndex != currIndex)
		{
			tokens.push_back(rules.squareBracetRule.build(input, currIndex, nextIndex));
			currIndex = nextIndex;
			continue;
		}
		nextIndex = rules.parenthesisRule.checkIfToken(input.c_str(), currIndex);
		if (nextIndex != currIndex)
		{
			tokens.push_back(rules.parenthesisRule.build(input, currIndex, nextIndex));
			currIndex = nextIndex;
			continue;
		}
		nextIndex = rules.expressionEndRule.checkIfToken(input.c_str(), currIndex);
		if (nextIndex != currIndex)
		{
			tokens.push_back(rules.expressionEndRule.build(input, currIndex, nextIndex));
			currIndex = nextIndex;
			continue;
		}
		nextIndex = rules.braceRule.checkIfToken(input.c_str(), currIndex);
		if (nextIndex != currIndex)
		{
			tokens.push_back(rules.braceRule.build(input, currIndex, nextIndex));
			currIndex = nextIndex;
			continue;
		}
		nextIndex = rules.stringLiteralRule.checkIfToken(input.c_str(), currIndex);
		if (nextIndex != currIndex)
		{
			tokens.push_back(rules.stringLiteralRule.build(input, currIndex, nextIndex));
			currIndex = nextIndex;
			continue;
		}
		nextIndex = rules.commentRule.checkIfToken(input.c_str(), currIndex);
		if (nextIndex != currIndex)
		{
			// do not build any tokens for a comment
			currIndex = nextIndex;
			continue;
		}
		std::cout << "ERROR: unexpected symbol" << input[currIndex] << std::endl;
	}
	tokens.push_back({ TokenType::EndOfStream, "END_OF_STREAM" });
	if (g_trace)
	{
		std::cout << "TOKENIZER RESULT: ";
		for (auto token : tokens)
		{
			std::cout << "(" << static_cast<int>(token.type) << ", \"" << token.content << "\"), ";
		}
		std::cout << std::endl;
	}
}



std::vector<Token> Lexer::convertToPostfix(const std::vector<Token>& tokens)
{
	std::vector<Token> convertedTokens;



	return convertedTokens;
}
/*
std::vector<BytecodeItem> Lexer::translate(const std::vector<Token>& tokens)
{

}
*/
//TODO: implement bytecode caching
std::unique_ptr<ASTNode> Lexer::getAST(const std::string& input)
{
	tokenize(input);
	return std::move(buildAST());
}

int Runtime::execute(const std::vector<BytecodeItem>& bytecode)
{
	return 0;
}

//TODO: process ifs
int FairytaleCore::interpret(const std::string& input)
{
	return m_runtime.execute_ast(m_lexer.getAST(input));
}

std::unique_ptr<ASTNode> FairytaleCore::loadAST(const std::string& input)
{
	return std::move(m_lexer.getAST(input));
}

int FairytaleCore::interpret(const char* line)
{
	std::string input = line;
	return interpret(input);
}

int FairytaleCore::interpret_file(const char* filename)
{
	std::ifstream file;
	file.open(filename);
	std::string input;

	std::stringstream sstr;

	while (file >> sstr.rdbuf());

	input = sstr.str();

	int errorCode = interpret(input);
	if (errorCode)
		return errorCode;

//	while (file)
//	{
//		std::getline(file, input);
//	}
}


Runtime::Runtime(FairytaleCore* pCore)
	: m_core(pCore)
	, globalScopeObject(-1)
{
	globalScopeObject = allocate_empty();
	inc_ref(globalScopeObject);
	register_global_function("pow", pow_wrapper);
	register_global_function("+", sum_wrapper);
	register_global_function("-", wrap<long long, long long, long long, sub>());
	register_global_function("*", wrap<long long, long long, long long, mul > ());
	register_global_function("/", wrap<long long, long long, long long, __div__>());
	register_global_function("%", wrap<long long, long long, long long, __mod__>());
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
	register_global_function("getattr", getattr);
	register_global_function("setattr", setattr);
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

	return std::move(m_core->loadAST(input));
}

int Runtime::execute_ast(std::unique_ptr<ASTNode>&& ast)
{
	loadedModuleASTs.emplace_back(std::move(ast));
	loadedModuleASTs.back()->execute(this);
	return 0;
}



std::unique_ptr<ASTNode> Lexer::buildAST()
{
	return std::move(ParseFileRoot());
}

std::string unescape(const std::string& s)
{
	std::string res;
	std::string::const_iterator it = s.begin();
	while (it != s.end())
	{
		char c = *it++;
		if (c == '\\' && it != s.end())
		{
			switch (*it++) {
			case '\\': c = '\\'; break;
			case 'n': c = '\n'; break;
			case 't': c = '\t'; break;
				// all other escapes
			default:
				// invalid escape sequence - skip it. alternatively you can copy it as is, throw an exception...
				continue;
			}
		}
		res += c;
	}

	return res;
}

std::unique_ptr<ASTNode> Lexer::ParseStringExpr() {
	auto Result = std::make_unique<StringASTN>(unescape(tokens[currentToken].content));
	currentToken++;
	return std::move(Result);

}


std::unique_ptr<ASTNode> Lexer::ParseNumberExpr() {
	auto Result = std::make_unique<NumberASTN>(tokens[currentToken].content);
	currentToken++;
	return std::move(Result);
}

std::unique_ptr<ASTNode> Lexer::ParseParenExpr() {
	currentToken++;
	auto V = ParseExpression();
	if (!V)
		return nullptr;

	if (tokens[currentToken].content != ")")
		assert(!"Expected )");
	currentToken++;
	return std::move(V);
}

std::unique_ptr<ASTNode> Lexer::ParseBooleanExpr() {
	const std::string& IdName = tokens[currentToken].content;
	currentToken++;
	if (IdName == "true")
		return std::make_unique<BooleanASTN>(true);
	if (IdName == "false")
		return std::make_unique<BooleanASTN>(false);
	assert(!"boolean expression is not supported");
	return nullptr;
}

std::unique_ptr<ASTNode> Lexer::ParseIdentifierExpr() {
	const std::string& IdName = tokens[currentToken].content;

	currentToken++;

	return std::make_unique<VariableASTN>(IdName);
}



std::unique_ptr<ASTNode> Lexer::ParseIf() {
	currentToken++;
	auto condition = std::make_unique<ConditionASTN>();
	condition->setCondition(ParseExpression());
	condition->setTrueBlock(ParseStatement());
	while ((tokens[currentToken].type == TokenType::ExpressionEnd)) currentToken++;
	if (tokens[currentToken].type == TokenType::Else)
	{
		currentToken++;
		condition->setFalseBlock(ParseStatement());
	}
	return std::move(condition);
}

std::vector<std::string> Lexer::ParseFunctionSignature() {
	assert(tokens[currentToken].type == TokenType::OpenParenthesis);
	currentToken++;
	std::vector<std::string> args;
	while (tokens[currentToken].type != TokenType::ClosedParenthesis)
	{
		if (tokens[currentToken].type == TokenType::Comma)
		{
			currentToken++;
		}
		assert(tokens[currentToken].type == TokenType::Identifier);
		args.push_back(tokens[currentToken].content);
		currentToken++;
		if (tokens[currentToken].type == TokenType::Comma)
		{
			currentToken++;
		}
	}
	currentToken++;
	return args;
}


std::unique_ptr<ASTNode> Lexer::ParseFunctionDeclaration() {
	currentToken++;
	auto func = std::make_unique<FuncASTN>();
	func->setSignature(ParseFunctionSignature());
	func->setFunctionBody(ParseStatement());
	return std::move(func);
}

std::unique_ptr<ASTNode> Lexer::ParseWhile() {
	currentToken++;
	auto loop = std::make_unique<WhileASTN>();
	loop->setCondition(ParseExpression());
	loop->setInnerBlock(ParseStatement());
	return std::move(loop);
}

std::unique_ptr<ASTNode> Lexer::ParseReturn() {
	currentToken++;
	auto loop = std::make_unique<ReturnASTN>();
	loop->setExpression(ParseExpression());
	return std::move(loop);
}


std::unique_ptr<ASTNode> Lexer::ParseBlock() {
	currentToken++;

	auto sequence = std::make_unique<SequenceASTN>();
	while (tokens[currentToken].content != "}")
	{
		auto V = ParseStatement();
		sequence->push_back(std::move(V));
		while (tokens[currentToken].type == TokenType::ExpressionEnd)
			currentToken++;
	}
	currentToken++;

	return std::move(sequence);
}

std::unique_ptr<ASTNode> Lexer::ParseFileRoot()
{
	auto sequence = std::make_unique<SequenceASTN>();
	while (tokens[currentToken].type != TokenType::EndOfStream)
	{
		auto V = ParseStatement();
		sequence->push_back(std::move(V));
	}
	currentToken++;
	return std::make_unique<RootASTN>(std::move(sequence));
}

std::unique_ptr<ASTNode> Lexer::ParseStatement()
{
	while (tokens[currentToken].type == TokenType::ExpressionEnd)
		currentToken++;
	if (tokens[currentToken].type == TokenType::OpenBrace)
	{
		return std::move(ParseBlock());
	}
	if (tokens[currentToken].type == TokenType::If)
	{
		return std::move(ParseIf());
	}
	if (tokens[currentToken].type == TokenType::While)
	{
		return std::move(ParseWhile());
	}
	if (tokens[currentToken].type == TokenType::Return)
	{
		return std::move(ParseReturn());
	}
	if (tokens[currentToken].type == TokenType::EndOfStream)
	{
		return std::make_unique<ASTNode>();
	}

	return std::move(ParseExpression());
}

std::unique_ptr<ASTNode> Lexer::ParsePrimary() {
	switch (tokens[currentToken].type) {
	default:
		assert(!"unknown token when expecting an expression");
	case TokenType::BooleanLiteral:
		return ParseBooleanExpr();
	case TokenType::Identifier:
		return ParseIdentifierExpr();
	case TokenType::Number:
		return ParseNumberExpr();
	case TokenType::String:
		return ParseStringExpr();
	case TokenType::OpenParenthesis:
		return ParseParenExpr();
	case TokenType::Func:
		return ParseFunctionDeclaration();
	case TokenType::BinaryOperator:
	//	return ParsePrefixUnaryOperator();
	case TokenType::ExpressionEnd:
		return nullptr;
	}
}

std::unique_ptr<ASTNode> Lexer::ParsePrefixUnaryOperator() {

	std::string currOp = tokens[currentToken].content;
	auto opAST = std::make_unique<PrefixOperatorExprASTN>(currOp);
	++currentToken;
	auto RHS = ParsePrimary();
	opAST->setRHS(std::move(RHS));
	return std::move(opAST);
}

std::unique_ptr<ASTNode> Lexer::ParseExpression() {
	auto LHS = ParsePrimary();
	if (!LHS && tokens[currentToken].type != TokenType::BinaryOperator)
		return nullptr;

	return ParseBinaryOperatorRHS(0, std::move(LHS));
}

std::unique_ptr<ASTNode> Lexer::ParseBinaryOperatorRHS(int ExprPrec,
	std::unique_ptr<ASTNode> LHS) {
	// If this is a binop, find its precedence.
	while (true) {
		if (tokens[currentToken].type == TokenType::ExpressionEnd)
		{
			currentToken++;
			return LHS;
		}
		std::string currOp = tokens[currentToken].content;
		if (!LHS)
		{
			currOp += "prefix";
		}
		int TokPrec = rules.GetOperatorPrecedence(currOp);

		// If this is a binop that binds at least as tightly as the current binop,
		// consume it, otherwise we are done.
		if (TokPrec < ExprPrec)
			return LHS;


		std::unique_ptr<ASTNode> RHS;
		if (tokens[currentToken].type == TokenType::OpenParenthesis)
		{
			currentToken++;  // eat (
			std::vector<std::unique_ptr<ASTNode>> Args;
			while (tokens[currentToken].type != TokenType::ClosedParenthesis) {
				if (auto Arg = ParseExpression())
					Args.push_back(std::move(Arg));
				else
					return nullptr;

				if (tokens[currentToken].type == TokenType::ClosedParenthesis)
					break;

				if (tokens[currentToken].type != TokenType::Comma)
					assert(!"Expected ')' or ',' in argument list");
				currentToken++;
			}

			// Eat the ')'.
			currentToken++;

			LHS = std::make_unique<CallExprASTN>(std::move(LHS), std::move(Args));
			continue;
		}
		else if (tokens[currentToken].type == TokenType::OpenSquareBrace)
		{
			currentToken++;  // eat (
			std::vector<std::unique_ptr<ASTNode>> Args;
			while (tokens[currentToken].type != TokenType::ClosedSquareBrace) {
				if (auto Arg = ParseExpression())
					Args.push_back(std::move(Arg));
				else
					return nullptr;

				if (tokens[currentToken].type == TokenType::ClosedSquareBrace)
					break;

				if (tokens[currentToken].type != TokenType::Comma)
					assert(!"Expected ')' or ',' in argument list");
				currentToken++;
			}

			// Eat the ')'.
			currentToken++;

			LHS = std::make_unique<IndexExprASTN>(std::move(LHS), std::move(Args));
			continue;
		}
		else
		{
			currentToken++;
			// Parse the primary expression after the binary operator.
			RHS = ParsePrimary();
			if (!RHS)
			{
				if (tokens[currentToken].type != TokenType::BinaryOperator)
					return nullptr;
				else
				{
					RHS = ParseBinaryOperatorRHS(TokPrec + 1, nullptr);
				}
			}
			
		}

		// If BinOp binds less tightly with RHS than the operator after RHS, let
		// the pending operator take RHS as its LHS.
		if (tokens[currentToken].type != TokenType::ExpressionEnd)
		{
			int NextPrec = rules.GetOperatorPrecedence(tokens[currentToken].content);
			if (TokPrec < NextPrec) {
				RHS = ParseBinaryOperatorRHS(TokPrec + 1, std::move(RHS));
				if (!RHS)
					return nullptr;
			}
		}

		// Merge LHS/RHS.
		if (currOp == ".")
		{
			LHS = std::make_unique<AccessAttribASTN>(currOp, std::move(LHS),
				std::move(RHS));
		}
		else if (currOp == "(")
		{

		}
		else
		{
			LHS = std::make_unique<BinaryExprASTN>(currOp, std::move(LHS),
				std::move(RHS));
		}


	}
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
		assert(!"Object is not callable");
}

