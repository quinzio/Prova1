int a_globalvariable = 2;
int b_globalvariable = 2;
struct ss{
    int c_globalvariable;
    int e_globalvariable;
}ss1;

int main() {
    int d_localvariable = 10;
    ss1.c_globalvariable = 2;
    if ( a_globalvariable && b_globalvariable && ss1.c_globalvariable && d_localvariable) {
        ss1.c_globalvariable = 1;
        ss1.e_globalvariable = 3;
    }
    a_globalvariable = b_globalvariable;
    return 0;
}