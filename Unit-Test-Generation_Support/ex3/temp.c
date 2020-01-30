int a, b;
int c[3] = {1, 2, 3};
int d[3];
int* p;

struct s {
    int a;
    char b;
}s2[3];

struct st {
    int c;
    char d;
    struct s s3;
}st1;


int main() {
    s2[0].a = 1;
    return 0;
}