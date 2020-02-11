// ex11
// Builtin types

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

	int res;

int main()
{
	c = 0x87878787ULL;
	uc = 0x87878787ULL;
	s = 0x87878787ULL;
	us = 0x87878787ULL;
	i = 0x87878787ULL;
	ui = 0x87878787ULL;
	ll = 0x87878787ULL;
	ul = 0x87878787ULL;
	l = 0x87878787ULL;
	ull = 0x87878787ULL;
	if (ul < l) res = 0;

	return 0;
}