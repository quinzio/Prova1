// ex19
// Now let's focus on function call.


 int f2(int a, char b) {
    return a + b;
}

int res;
int res1;

int f1(int a, char b) {
    res = f2(3, 1005);
    res1 = res;
    return res;
}