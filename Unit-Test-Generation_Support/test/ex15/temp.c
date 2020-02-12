// ex15
// The infamous bitfields. 
// 


// For structs it's easy
struct sa {
	unsigned char a:1;
	unsigned char b:1;
	unsigned char c:2;
}s1;

// Now for unions, made of structs !!!
union u {
    unsigned char uc1; 
    struct sb {
    	unsigned char a:1;
    	unsigned char b:1;
    	unsigned char c:2;
    }s2;
}u1;

int f1()
{
    s1.a = 1;
    s1.b = 0;
    s1.c = 2;
    u1.uc1 = 0x55;
	return 0;
}
int f2()
{
    u1.uc1 = 0x56;
	return 0;
}
int f3()
{
    u1.uc1 = 0x57;
	return 0;
}
int f4()
{
    u1.uc1 = 0x58;
	return 0;
}
int f5()
{
    u1.uc1 = 0x50;
	return 0;
}

