
#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <iomanip>



int main()
{
	std::ifstream infile("Typedef.txt");
	std::string str;
	std::regex reg(
		"[^\\w<]*"
		"[\\w<]+"
		"\\s"
		"0x[\\da-f]{6,11}"
		"\\s"
		"<[^>]*>"
		"\\s"
		"(?:col:\\d+|line:\\d+:\\d+)"
		"(?:\\sreferenced)?"
		"\\s"
		"(\\w+)"
		"\\s"
		"'([^']+)'"
		"(?::'([^']+)')?"
	);
	std::smatch sm;
	int ix = 0;
	while (std::getline(infile, str)) {
		if (std::regex_search(str, sm, reg)) {
			std::cout << "Matched" << std::setw(5) << ix << std::setw(20) << sm[1] << std::setw(30) << sm[2] << std::setw(30) << sm[3] << "\n";
		}
		else {
			std::cout << "*******" << std::setw(5) << ix << std::setw(20) << sm[1] << std::setw(30) << sm[2] << std::setw(30) << sm[3] << "\n";
		}
		ix++;

	}


}

