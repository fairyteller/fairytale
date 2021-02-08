#include "fairytale.h"
#include "fairytale_core.h"
#include <iostream>
#include <fstream>
#include <string>


Fairytale::Fairytale() : m_pCore(nullptr)
{
	m_pCore = new FairytaleCore();
}

Fairytale::~Fairytale()
{
	delete m_pCore;
}

int Fairytale::interpret(const char* filename, const char* line)
{
	std::string* sFilename = new std::string(filename);
	return m_pCore->interpret(*sFilename, line);
}

int Fairytale::interpret_file(const char* filename)
{
	return m_pCore->interpret_file(filename);
}

Runtime* Fairytale::get_runtime()
{
	return m_pCore->get_runtime();
}

void Fairytale::set_io_handler(IFileSystemAbstraction* ioHandler)
{
	m_pCore->set_io_handler(ioHandler);
}