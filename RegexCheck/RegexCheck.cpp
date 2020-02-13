
#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdint>

#include <iostream>
#include <regex>
#include <string>

std::regex e;

std::string fail("int *(((([1][2][3][4][5]");

int main()
{
	std::string str("aaa");
	e = std::regex(str+str+ std::to_string(1));
	return 0;
}
