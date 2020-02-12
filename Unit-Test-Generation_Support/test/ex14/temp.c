// ex14
// Structures with member 
// of type Typedef'd


typedef unsigned int u16;
typedef unsigned char u8;

struct s_a {
	int abc;
};

struct s {
	u16 var1;
	u8 var2[10];
	struct s_a s_a1;
}s1;


int main()
{
	s1.var2[3] = 1;
	s1.var2[3] = 2;
	s1.var2[3] = 3;
	s1.var2[4] = 4;
	return 0;
}