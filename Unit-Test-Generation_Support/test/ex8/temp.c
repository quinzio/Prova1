// ex8
// Typedef
// Named structures
// Anon. structures, typedef is declared just after the struct
// Plain typedef 

struct a{
    int fa;
};
typedef struct {
    int fb;
};
typedef int myint;
typedef myint myint2;
typedef struct a sa;
typedef sa tsa;
typedef struct {
    int fb;
}sb, sb2;
typedef sb tsb;
typedef int a1[3];
typedef a1 *a2;
typedef a2 a3[5];
a3 va3;
typedef int *(*(   *(a4[5])));
int f1(){
    myint vv1;
    myint2 vv2;
    tsa vv3;
    sb vv4;
    a1 vv5;
    a3 vv6;
    vv1 = 1;
    vv2 = 2;
    vv3.fa = 3;
    vv4.fb = 4;
    vv5[2] = 5;
    vv6[4] = &vv5;
    a3[4] = &vv5;
    
   return 0;
}