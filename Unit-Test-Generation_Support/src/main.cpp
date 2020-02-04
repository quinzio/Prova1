#include "Unit-Test-Generation-Support.h"
#include <iostream>
#include <string>
#include <fstream>
#include "main.h"

int main(int argc, char* argv[]) {
	std::string testFolder;
	try
	{
		testCompare("ex1");
		testCompare("ex2");
		testCompare("ex3");
		testCompare("ex4");
		testCompare("ex5");
		testCompare("ex6");
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}
}

void testCompare(std::string testFolder)
{
	std::string inner_argv[3];
	std::ifstream resultFile("test/" + testFolder + "/result.txt");
	std::ifstream expectedFile("test/" + testFolder + "/expected.txt");
	inner_argv[1] = ("test/" + testFolder + "/ast.txt").c_str();
	inner_argv[2] = ("test/" + testFolder + "/result.txt").c_str();
	int	argc = 3;
	cleanTestStorage();
	inner_main(argc, inner_argv);
	std::cout << "Comparing results for test " << testFolder << "\n";
	if (resultFile.is_open() && expectedFile.is_open()) {
		std::string resultStr;
		std::string expectedStr;
		bool stringCompareFailure = false;
		bool resultEof, expectedEof;
		while (1) {
			std::getline(resultFile, resultStr);
			resultEof = resultFile.rdstate() & std::ifstream::eofbit;
			std::getline(expectedFile, expectedStr);
			expectedEof = expectedFile.rdstate() & std::ifstream::eofbit;
			if (resultEof || expectedEof)
			{
				break;
			}
			if (resultStr.compare(expectedStr) != 0) {
				stringCompareFailure = true;
				break;
			}
		}
		if ((resultEof != expectedEof) || stringCompareFailure) {
			std::cout << "\tFailed test in folder " << testFolder << "\n";
			if (resultEof != expectedEof) {
				std::cout << "\tFile length is different.\n";
			}
			if (stringCompareFailure) {
				std::cout << "\tString compare failed.\n";
			}
		}
		else {
			std::cout << "\tTest " << testFolder << " OK" << ".\n";
		}
	}
	else {
		std::cout << "\tProblems opening result or expected files.\n";
	}
}
