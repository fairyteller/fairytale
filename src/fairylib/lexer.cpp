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