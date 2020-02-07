// ex10
// Unions

union u1 {
	int a;
	long b;
}vu1;
/*
union u2 {
	int c;
	long d;
	union u1 v2u1;
}vu2;
*/
int main()
{
	vu1.a = 1111;
	//vu2.c = 1111;
	return 0;
}