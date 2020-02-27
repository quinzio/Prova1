int f1(int f1a, int f1b, int f1c);
int f2(int f2a, int f2b, int f2c);
int f3(int f3a, int f3b, int f3c);

int a;
int b;
int c;

int fun1(int par1) {
	f1(1, 2, par1);
	a = f2(10, 20, par1);
	b = f3(11, 12, par1);
    if (a > b) {
        c = a+1;
    }
	return 0;
}