int a, b;
int c[3] = {1, 2, 3};
int d[3];
int* p;

int(*(*(*((*foo)(int, char(*)[2])))[4])[4])[4];

union u {
    int a;
    char b;
}u2;

struct s {
    int a;
    char b;
}s2;

struct st {
    int c;
    char d;
    struct s s3;
}st1;


int main() {
    struct s* s4[4];
    struct s(*s5[3])[4];
    int i;
    int * i1;
    s2.a = 1;
    return 0;
}