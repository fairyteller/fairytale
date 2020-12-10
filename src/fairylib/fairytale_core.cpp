#include "fairytale_core.h"
#include "standard_library.h"
#include "runtime.h"
#include "abstract_syntax_tree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//TODO: process ifs
int FairytaleCore::interpret(const std::string& input)
{
	m_runtime.push_frame();
	int result = m_runtime.execute_ast(m_lexer.getAST(input));
	if (result)
	{
		ObjectRef exception(&m_runtime, m_runtime.copy_from_register());
		std::cerr << "Unhandled Exception: " << std::endl << exception->toString(&m_runtime);
		m_runtime.load_from_register();
	}
	m_runtime.pop_frame();
	return result;
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
