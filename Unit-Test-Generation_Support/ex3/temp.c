int a, b;
int c[3] = {1, 2, 3};
int d[3];
int* p;

#define MASK c[2]


int main() {
    b = MASK;
    c[1] = b;
    return 0;
}