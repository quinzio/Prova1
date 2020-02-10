
#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <iomanip>


class Test {
private:
	int a;
	int b;

public:
	Test& operator=(const Test& rhs) {
		*this = rhs;
		return *this;
	}

};

int main()
{
	Test a, b;

	a = b;

	return 0;
}

