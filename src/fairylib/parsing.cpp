#include "parsing.h"


size_t readWhileFiltered(const std::string& string, size_t startIndex, const ISymbolFilter& filter)
{
	size_t index = startIndex;
	while (filter.check(string[index]))
	{
		index++;
	}
	return index;
}

size_t readWhileNotFiltered(const std::string& string, size_t startIndex, const ISymbolFilter& filter)
{
	size_t index = startIndex;
	while (!filter.check(string[index]))
	{
		index++;
	}
	return index;
}
