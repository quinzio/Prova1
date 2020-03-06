
int a, b, c, d, e, f;

const unsigned int k1 = 9;
const unsigned int k2[2] = {3, 4};
int const * k3;


struct ts1{
    const int ts1a;
    int ts1b;
} vs1;


int fun1(int par1) {
    k3 = &a;
    vs1.ts1b = 1;
	return 0;
}


