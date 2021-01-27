#include "lexer.h"
#include "config.h"
#include "parsing.h"
#include "abstract_syntax_tree.h"
#include <iostream>
#include <sstream>

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
		nextIndex = rules.floatRule.checkIfToken(input.c_str(), currIndex);
		if (nextIndex != currIndex)
		{
			tokens.push_back(rules.floatRule.build(input, currIndex, nextIndex));
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
	if (gTRACE)
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
	if (strstr(tokens[currentToken].content.c_str(), ".") == 0)
	{
		auto Result = std::make_unique<NumberASTN>(tokens[currentToken].content);
		currentToken++;
		return std::move(Result);
	}
	else
	{
		auto Result = std::make_unique<FloatASTN>(tokens[currentToken].content);
		currentToken++;
		return std::move(Result);
	}
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



std::unique_ptr<ASTNode> Lexer::ParseIfElse() {
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

std::unique_ptr<ASTNode> Lexer::ParseTryCatchFinally() {
	currentToken++;
	auto astn = std::make_unique<TryCatchFinallyASTN>();
	astn->setTryBlock(ParseStatement());
	while ((tokens[currentToken].type == TokenType::ExpressionEnd)) currentToken++;
	if (tokens[currentToken].type == TokenType::Catch)
	{
		currentToken++;
		astn->setCatchBlock(ParseStatement());
	}
	return std::move(astn);
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
	auto ret = std::make_unique<ReturnASTN>();
	ret->setExpression(ParseExpression());
	return std::move(ret);
}

std::unique_ptr<ASTNode> Lexer::ParseThrow() {
	currentToken++;
	auto throwASTN = std::make_unique<ThrowASTN>();
	throwASTN->setExpression(ParseExpression());
	return std::move(throwASTN);
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
		return std::move(ParseIfElse());
	}
	if (tokens[currentToken].type == TokenType::Try)
	{
		return std::move(ParseTryCatchFinally());
	}
	if (tokens[currentToken].type == TokenType::While)
	{
		return std::move(ParseWhile());
	}
	if (tokens[currentToken].type == TokenType::Return)
	{
		return std::move(ParseReturn());
	}
	if (tokens[currentToken].type == TokenType::Throw)
	{
		return std::move(ParseThrow());
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
