
#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <iomanip>

union u1 {
	int a;
	long b;
}vu1;
union u2 {
	int c;
	long d;
	union u1 v2u1;
}vu2;

int main()
{
	vu1.a = 1111;
	vu2.c = 1111;
	std::cout << std::setw(20) << "vu1.a " << vu1.a << "\n";
	std::cout << std::setw(20) << "vu1.b " << vu1.b << "\n";
	std::cout << std::setw(20) << "vu2.v2u1.a " << vu2.v2u1.a << "\n";
	std::cout << std::setw(20) << "vu2.v2u1.b " << vu2.v2u1.b << "\n";
	std::cout << std::setw(20) << "vu2.c " << vu2.c << "\n";
	std::cout << std::setw(20) << "vu2.d " << vu2.d << "\n";
	return 0;
}

