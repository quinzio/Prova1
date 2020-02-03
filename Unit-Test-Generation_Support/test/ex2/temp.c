int a, b;
int c[3] = {1, 2, 3};
int d[3];
int* p;

struct s {
    int a;
    char b;
}s2;

struct st {
    int c;
    char d;
    struct s s3;
}st1;

struct s* ps[4];
struct s s0[4];
struct s s1;
struct s s3;
struct s s4;

int main() {
    struct s s2;
    ps[0] = &s1;
    ps[1] = &s2;
    ps[2] = &s3;
    ps[3] = &s4;
    s0[3].a = 1;
    ps[0]->a = 1;
    ps[0]->b = 2;
    ps[1]->a = 3;
    ps[1]->b = 4;
    ps[2]->a = 5;
    ps[2]->b = 6;
    ps[3]->a = 7;
    ps[3]->b = 8;
    return 0;
}