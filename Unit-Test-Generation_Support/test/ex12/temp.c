// ex11
// Unions
struct s{
        int vInt;
        long vLong;
}s1;

struct u {
    char c;
    unsigned char uc;
	short s;
	unsigned short us;
	int i;
	unsigned int ui;
	long l;
	unsigned long ul;
	long long ll;
	unsigned long long ull;
    struct s s2;
}vu;



int main()
{
	vu.c = 0x87878787ULL;
	vu.uc = 0x87878787ULL;
	vu.s = 0x87878787ULL;
	vu.us = 0x87878787ULL;
	vu.i = 0x87878787ULL;
	vu.ui = 0x87878787ULL;
	vu.ll = 0x87878787ULL;
	vu.ul = 0x87878787ULL;
	vu.l = 0x87878787ULL;
	vu.ull = 0x87878787ULL;
    s1.vInt = vu.b;
	return 0;
}