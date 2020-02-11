
#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdint>


// ex11
// Unions
struct s {
	int vInt;
	long vLong;
}s1;

union u {
	char c;
	unsigned char uc;
	short s;
	unsigned short us;
	int i;
	unsigned int ui;
	long b;
	unsigned long ul;
	long long ll;
	unsigned long long ull;
	struct s s2;
}vu;
/*
union u2 {
	int c;
	long d;
	union u1 v2u1;
}vu2;
*/
int main()
{
	vu.ull = 0x87878787;
	s1.vInt = vu.b;
	return 0;
}
