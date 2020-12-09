#pragma once
#include "parsing.h"
#include "fairy_object.h"
#include "runtime.h"
#include "rules.h"
#include "lexer.h"
#include "abstract_syntax_tree.h"
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



