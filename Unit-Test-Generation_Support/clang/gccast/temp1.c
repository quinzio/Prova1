int a = 0;
int b = 0;
struct sa{
    int sam1;
    int sam2;
} sa1;
struct sb{
    int sbm1;
    int sbm2;
    struct sa sa2;
} sb1;

int main() {
    int c = 0;
    if ((a == 0 && b > 10) || (c < 1)) { 
        sa1.sam1 = 1;
    }
    else {
        b = 2;
        sa1.sam2 = 1;
        sb1.sa2.sam1 = 3;
    }
    return 0;
}