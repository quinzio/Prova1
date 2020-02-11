// ex11
// Unions
struct s{
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



int main()
{
	vu.ull = 0x87878787ULL;
	vu.ll = 0x87878787ULL;
	vu.ui = 0x87878787ULL;
	vu.i = 0x87878787ULL;
	vu.uc = 0x87878787ULL;
	vu.c = 0x87878787ULL;
    s1.vInt = vu.b;
	return 0;
}