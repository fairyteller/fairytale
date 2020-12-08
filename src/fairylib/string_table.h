#pragma once
#include "string_table.h"
#include <vector>
#include <string>
#include <unordered_map>

typedef size_t stringId;

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