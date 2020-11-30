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

int Fairytale::interpret(const char* line)
{
	return m_pCore->interpret(line);
}

int Fairytale::interpret_file(const char* filename)
{
	return m_pCore->interpret_file(filename);
}