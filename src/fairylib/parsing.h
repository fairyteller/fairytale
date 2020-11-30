#pragma once
#include <algorithm>
#include <vector>
#include <string>

class SymbolRange
{
public:
	SymbolRange() : start(0), end(0) {}
	SymbolRange(char symbol) : start(symbol), end(symbol+1) {}
	SymbolRange(char fromSymbol, char toSymbol) : start(fromSymbol), end(toSymbol + 1) {}

	bool contains(char character)
	{
		return character >= start && character < end;
	}

	SymbolRange intersect(const SymbolRange& otherRange)
	{
		if (otherRange.start > end || otherRange.end < start)
			return SymbolRange();
		return SymbolRange(std::max(start, otherRange.start), std::min(end, otherRange.end));
	}
private:
	char start;
	char end;
};

class ISymbolFilter
{
public:
	virtual ~ISymbolFilter() {}
	virtual bool check(char character) const = 0;
};

class RangeBasedFilter : public ISymbolFilter
{
public:
	RangeBasedFilter& set(char character)
	{
		ranges.clear();
		ranges.push_back(SymbolRange(character));
		return *this;
	}
	RangeBasedFilter& set(SymbolRange range)
	{
		ranges.clear();
		ranges.push_back(range);
		return *this;
	}

	RangeBasedFilter& unite(SymbolRange range)
	{
		ranges.push_back(range);
		return *this;
	}

	RangeBasedFilter& intersect(SymbolRange otherRange)
	{
		for (auto& range : ranges)
			range = range.intersect(otherRange);
		return *this;
	}
	virtual bool check(char character) const
	{
		for (auto range : ranges)
			if (range.contains(character))
				return true;
		return false;
	}

private:
	std::vector<SymbolRange> ranges;
};

size_t readWhileFiltered(const std::string& string, size_t startIndex, const ISymbolFilter& filter);

size_t readWhileNotFiltered(const std::string& string, size_t startIndex, const ISymbolFilter& filter);
enum class TokenType
{
	Unknown,
	Number,
	BinaryOperator,
	Identifier,
	String,
	BooleanLiteral,
	OpenParenthesis,
	ClosedParenthesis,
	OpenBrace,
	ClosedBrace,
	Comma,
	If,
	While,
	Else,
	ExpressionEnd,
	EndOfStream,
	Func,
	Import,
	Return
};

struct Token
{
	TokenType type;
	std::string content;
};

typedef Token(*TokenFactory)(const std::string&, size_t, size_t);

struct SymbolSequence
{
	size_t minAmount;
	size_t maxAmount;
	ISymbolFilter& filter;
};

class TokenRules
{
public:
	TokenRules() : currentFilterIndex(0), filterSpecificIndex(0), tokenFactory(nullptr)
	{

	}
	TokenRules& setFactory(TokenFactory factory)
	{
		tokenFactory = factory;
		return *this;
	}
	TokenRules& operator<<(SymbolSequence sequence)
	{
		allowedSymbolsSequence.push_back(sequence);
		return *this;
	}
	size_t checkIfToken(const char* input, size_t startIndex)
	{
		size_t index = startIndex;
		currentFilterIndex = 0;
		filterSpecificIndex = 0;
		while(currentFilterIndex < allowedSymbolsSequence.size())
		{
			if (allowedSymbolsSequence[currentFilterIndex].filter.check(input[index]))
			{
				index++;
				filterSpecificIndex++;
				if (filterSpecificIndex >= allowedSymbolsSequence[currentFilterIndex].maxAmount)
				{
					currentFilterIndex++;
					filterSpecificIndex = 0;
				}
			}
			else
			{
				if (filterSpecificIndex >= allowedSymbolsSequence[currentFilterIndex].minAmount && filterSpecificIndex <= allowedSymbolsSequence[currentFilterIndex].maxAmount)
				{
					currentFilterIndex++;
					filterSpecificIndex = 0;
				}
				else
				{
					return startIndex;
				}
			}
		}
		return index;
	}

	void reset()
	{

	}
	Token build(const std::string& input, size_t startIndex, size_t endIndex)
	{
		return tokenFactory(input, startIndex, endIndex);
	}
	std::vector<SymbolSequence> allowedSymbolsSequence;
	size_t currentFilterIndex;
	size_t filterSpecificIndex;
	TokenFactory tokenFactory;
};
