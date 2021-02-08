#pragma once
#include "runtime.h"
#include "rules.h"
#include <memory>
#include <vector>
#include <string>

class BytecodeItem {

};

class ASTNode;
class Lexer
{
public:
	std::vector<BytecodeItem> getBytecode(const std::string& input, Runtime* pRuntime);
	void tokenize(const std::string& filename, const std::string& input);
	std::vector<Token> convertToPostfix(const std::vector<Token>& tokens);
	std::unique_ptr<ASTNode> buildAST();
	std::unique_ptr<ASTNode> getAST(const std::string& filename, const std::string& input);
	std::vector<BytecodeItem> translate(const std::vector<Token>& tokens);


	std::unique_ptr<ASTNode> ParseFileRoot();
	std::unique_ptr<ASTNode> ParseStatement();
	std::unique_ptr<ASTNode> ParseBlock();
	std::unique_ptr<ASTNode> ParseIfElse();
	std::unique_ptr<ASTNode> ParseTryCatchFinally();
	std::unique_ptr<ASTNode> ParseWhile();
	std::unique_ptr<ASTNode> ParseReturn();
	std::unique_ptr<ASTNode> ParseThrow();
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