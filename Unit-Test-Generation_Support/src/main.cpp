#include "Unit-Test-Generation-Support.h"
#include <iostream>
#include <string>
#include <fstream>
#include "main.h"

int main(int argc, char* argv[]) {
	std::string testFolder;
	try
	{
		//testCompare("ex01");	getchar();
		//testCompare("ex02");	getchar();
		//testCompare("ex03");	getchar();
		//testCompare("ex04");	getchar();
		//testCompare("ex05");	getchar();
		//testCompare("ex06");	getchar();
		//testCompare("ex07");	getchar();
		//testCompare("ex08");	getchar();
		//testCompare("ex09");	getchar();
		//testCompare("ex10");	getchar();
		//testCompare("ex11");	getchar();
		//testCompare("ex12");	getchar();
		//testCompare("ex13");	getchar();
		//testCompare("ex14");	getchar();
		//testCompare("ex15");	getchar();
		//testCompare("ex16");	getchar();
		//testCompare("ex17");	getchar();
		//testCompare("ex18");	getchar();
		//testCompare("ex19");	getchar();
		//testCompare("ex20");	getchar();
		//testCompare("ex21");	getchar();
		testCompare("final");	getchar();

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
