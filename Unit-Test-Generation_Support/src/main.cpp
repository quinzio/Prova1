#include "Unit-Test-Generation-Support.h"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
	const char* inner_argv[3];
	try
	{
		argc = 3;
		inner_argv[1] = "test/ex1/ast.txt";
		inner_argv[2] = "test/ex1/result.txt";
		inner_main(argc, inner_argv);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}