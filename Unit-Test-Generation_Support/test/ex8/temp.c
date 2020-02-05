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
}sb;

typedef sb tsb;

typedef int a1[3];
typedef a1 *a2;
typedef a2 a3[5];
a3 va3;
typedef int *(*(   *(a4[5])));


int f1(){
        return 0;
}