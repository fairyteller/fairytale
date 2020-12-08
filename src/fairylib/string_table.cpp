#include "string_table.h"

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