#include "fairytale_core.h"
#include "standard_library.h"
#include "runtime.h"
#include "abstract_syntax_tree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

const char sUnhandledException[] = "Uncaught Exception raised: \n";

//TODO: process ifs
int FairytaleCore::interpret(const std::string& filename, const std::string& input)
{
	m_runtime.push_frame();
	int result = m_runtime.execute_ast(m_lexer.getAST(filename, input));
	if (result)
	{
		ObjectRef exception(&m_runtime, m_runtime.copy_from_register());
		std::string exceptionMessage = exception->toString(&m_runtime);
		std::string savedStackTrace = m_runtime.get_last_stack_trace();
		m_ioHandler->write(STDERR, reinterpret_cast<const void*>(sUnhandledException), sizeof(sUnhandledException));
		m_ioHandler->write(STDERR, reinterpret_cast<const void*>(exceptionMessage.c_str()), exceptionMessage.size());
		m_ioHandler->write(STDERR, reinterpret_cast<const void*>(savedStackTrace.c_str()), savedStackTrace.size());
		m_runtime.set_exception_as_handled();
		m_runtime.load_from_register();
	}
	m_runtime.pop_frame();
	return result;
}

std::unique_ptr<ASTNode> FairytaleCore::loadAST(const std::string& filename, const std::string& input)
{
	return std::move(m_lexer.getAST(filename, input));
}

int FairytaleCore::interpret(const std::string& filename, const char* line)
{
	std::string input = line;
	return interpret(filename, input);
}

int FairytaleCore::interpret_file(const char* filename)
{
	std::ifstream file;
	file.open(filename);
	std::string input;
	std::string* sFilename = new std::string(filename);

	std::stringstream sstr;

	while (file >> sstr.rdbuf());

	input = sstr.str();

	int errorCode = interpret(*sFilename, input);
	if (errorCode)
		return errorCode;

//	while (file)
//	{
//		std::getline(file, input);
//	}
}
