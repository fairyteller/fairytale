#pragma once
#include "parsing.h"
#include <map>

struct Rules
{
	RangeBasedFilter spacesAndTabs;
	RangeBasedFilter dot;
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
	RangeBasedFilter newLine;
	RangeBasedFilter quote;
	RangeBasedFilter stringContentSymbol;
	RangeBasedFilter numberSign;
	RangeBasedFilter commentContentSymbol;
	RangeBasedFilter commentEnd;

	TokenRules numberRule;
	TokenRules floatRule;
	TokenRules braceRule;
	TokenRules operatorRule;
	TokenRules identifierRule;
	TokenRules parenthesisRule;
	TokenRules expressionEndRule;
	TokenRules newLineRule;
	TokenRules stringLiteralRule;
	TokenRules commentRule;
	TokenRules squareBracetRule;

	std::map<std::string, int> binaryOperatorPrecedence;

	int GetOperatorPrecedence(const std::string& requestedOperator) {
		int TokPrec = binaryOperatorPrecedence[requestedOperator];
		if (TokPrec <= 0) return -1;
		return TokPrec;
	}

	static Token createNumberToken(const std::string& input, size_t startIndex, size_t endIndex, SourceCodePosition position)
	{
		Token token;
		token.type = TokenType::Number;
		token.content = input.substr(startIndex, endIndex - startIndex);
		token.sourceCodePosition = position;
		return token;
	}

	static Token createParenthesisToken(const std::string& input, size_t startIndex, size_t endIndex, SourceCodePosition position)
	{
		Token token;
		if (input[startIndex] == '(')
			token.type = TokenType::OpenParenthesis;
		if (input[startIndex] == ')')
			token.type = TokenType::ClosedParenthesis;
		if (input[startIndex] == ',')
			token.type = TokenType::Comma;
		token.content = input[startIndex];
		token.sourceCodePosition = position;
		return token;
	}

	static Token createBraceToken(const std::string& input, size_t startIndex, size_t endIndex, SourceCodePosition position)
	{
		Token token;
		if (input[startIndex] == '{')
			token.type = TokenType::OpenBrace;
		if (input[startIndex] == '}')
			token.type = TokenType::ClosedBrace;
		token.content = input[startIndex];
		token.sourceCodePosition = position;
		return token;
	}

	static Token createSquareBraceToken(const std::string& input, size_t startIndex, size_t endIndex, SourceCodePosition position)
	{
		Token token;
		if (input[startIndex] == '[')
			token.type = TokenType::OpenSquareBrace;
		if (input[startIndex] == ']')
			token.type = TokenType::ClosedSquareBrace;
		token.content = input[startIndex];
		token.sourceCodePosition = position;
		return token;
	}
	static Token createOperatorToken(const std::string& input, size_t startIndex, size_t endIndex, SourceCodePosition position)
	{
		Token token;
		token.type = TokenType::BinaryOperator;
		token.content = input.substr(startIndex, endIndex - startIndex);
		token.sourceCodePosition = position;
		return token;
	}

	static Token createIdentifierToken(const std::string& input, size_t startIndex, size_t endIndex, SourceCodePosition position)
	{
		Token token;
		token.content = input.substr(startIndex, endIndex - startIndex);
		token.sourceCodePosition = position;
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
		else if (assumedKeyword == "class")
		{
			token.type = TokenType::Class;
		}
		else if (assumedKeyword == "try")
		{
			token.type = TokenType::Try;
		}
		else if (assumedKeyword == "catch")
		{
			token.type = TokenType::Catch;
		}
		else if (assumedKeyword == "finally")
		{
			token.type = TokenType::Finally;
		}
		else if (assumedKeyword == "throw")
		{
			token.type = TokenType::Throw;
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

	static Token createStringLiteralToken(const std::string& input, size_t startIndex, size_t endIndex, SourceCodePosition position)
	{
		Token token;
		token.type = TokenType::String;
		token.content = input.substr(startIndex + 1, endIndex - startIndex - 2);
		token.sourceCodePosition = position;
		return token;

	}

	static Token createExpressionEndToken(const std::string& input, size_t startIndex, size_t endIndex, SourceCodePosition position)
	{
		Token token;
		token.type = TokenType::ExpressionEnd;
		token.content = input.substr(startIndex, endIndex - startIndex);
		token.sourceCodePosition = position;
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
		dot.set('.');
		operatorSymbols.set('+').unite('-').unite('*').unite('/').unite('=').unite('>').unite('<').unite('&').unite('|').unite('!').unite('%').unite('.').unite('@');
		parenthesis.set('(').unite(')').unite(',');
		brace.set('{').unite('}');
		squareBrace.set('[').unite(']');
		englishLetters.set({ 'A','Z' }).unite({ 'a', 'z' });
		validIdentifierFirstSymbol.set({ 'A','Z' }).unite({ 'a', 'z' }).unite('_');
		validIdentifierSymbol.set({ 'A','Z' }).unite({ 'a', 'z' }).unite('_').unite({ '0','9' });
		newLine.set('\n');
		expressionEnd.set(';').unite('\0').unite('\r');
		commentEnd.set('\n').unite('\0');
		quote.set('\"');
		numberSign.set('#');
		stringContentSymbol.set({ 1, '\"' - 1 }).unite({ '\"' + 1, 126 });
		commentContentSymbol.set({ 1, '\n' - 1 }).unite({ '\n' + 1, 126 });

		floatRule.setFactory(createNumberToken) << SymbolSequence{ 0, UINT32_MAX, digit } << SymbolSequence{ 1, 1, dot } << SymbolSequence{ 1, UINT32_MAX, digit };
		numberRule.setFactory(createNumberToken) << SymbolSequence{ 1, UINT32_MAX, digit };
		operatorRule.setFactory(createOperatorToken) << SymbolSequence{ 1, 2, operatorSymbols };
		identifierRule.setFactory(createIdentifierToken) << SymbolSequence{ 1, 1, validIdentifierFirstSymbol } << SymbolSequence{ 0, UINT32_MAX, validIdentifierSymbol };
		parenthesisRule.setFactory(createParenthesisToken) << SymbolSequence{ 1, 1, parenthesis };
		braceRule.setFactory(createBraceToken) << SymbolSequence{ 1, 1, brace };
		squareBracetRule.setFactory(createSquareBraceToken) << SymbolSequence{ 1, 1, squareBrace };
		expressionEndRule.setFactory(createExpressionEndToken) << SymbolSequence{ 1, 1, expressionEnd };
		newLineRule.setFactory(createExpressionEndToken) << SymbolSequence{ 1, 1, newLine };
		stringLiteralRule.setFactory(createStringLiteralToken) << SymbolSequence{ 1, 1, quote } << SymbolSequence{ 0, UINT32_MAX, stringContentSymbol } << SymbolSequence{ 1, 1, quote };
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