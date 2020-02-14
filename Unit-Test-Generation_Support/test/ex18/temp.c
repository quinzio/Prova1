// ex18
// More gibberish c declarations, including functions.

int var1 = 1, var2 = 2;

int (*(**f3)(long l1))(long l1);
int (*(*ff3)(long l1))(long l1);
int (*(*f4)(char))[4];

int (*(*(((**(f5)(char))[4])[5])[6]   )(long l1))(long l1);

int  *(*((((v1)[5])[4])[3])[2])[6][7];

int  ((((v2[1])[2])[3])[4])[5];
int *((((v3[1])[2])[3])[4])[5];
int (*(((v4[1])[2])[3])[4])[5];
int ((*((v5[1])[2])[3])[4])[5];
int (((*(v6[1])[2])[3])[4])[5];
int ((((*v7[1])[2])[3])[4])[5];

// Yes, it's legal
int ((*((((((*(*((v8[1])))(int (((*([2])[3])[4])[5])[6], int (((*([2])[3])[4])[5])[6]))[2])[3])[4])[5][1])[2])[3])[4])[5];
int ((*((((((*(*(v9[1]))(int (((([2])[3])[4])[5])[6], int (((*([2])[3])[4])[5])[6]))[2])[3])[4])[5][1])[2])[3])[4])[5];

int f2(int a){
        return a+1;
}
int (*f6(long l1))(long l1){
        int (*k)(long l1) = 0;
        return k;
}

int f1(int a, char b) {
    var1 = a;
    var2 = b;
    f3 = &ff3;
    *f3 = &f6;
    (*f3/*comment here*/)(1000000);
    f2(10);
    return var1;
}