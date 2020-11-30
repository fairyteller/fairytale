#include "fairytale.h"
#include <string>
#include <iostream>

int main(int argc, char** argv)
{
	Fairytale core;
	if (argc == 1)
	{
		std::string input;
		while (true)
		{
			std::cout << ">>> ";
			std::getline(std::cin, input);
			core.interpret(input.c_str());
		}
	}
	else
	{
		core.interpret_file(argv[1]);
	}
}