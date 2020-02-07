// ex7
// The dreadful case 7, this is mostly to see if it "runs"
// not really to compare the variables values.
// Nested types on variable declaration


typedef int  ***(*(((*(*tvar1[5]))[4])[3])[2])[1][2][3];
typedef int  ***(*(((*  tvar2    )[4])[3])[2])[1][2][3];
typedef int  ***(*((    tvar3     [4])[3])[2])[1][2][3];
typedef int (***(*((     tvar3B   [4])[3])[2])[1][2][3])(int);
typedef int  ***        tvar4                 [1][2][3];
typedef int  **         tvar5                          ;
typedef int  *          tvar6                          ;
typedef int             tvar7                          ;

tvar1 var1;
tvar2 var2;
tvar3 var3;
tvar4 var4;
tvar5 var5;
tvar6 var6;
tvar7 var7;

int f1(){
    var1[1]         = &var2;
    var2            = &var3;
    var3[1][1][1]   = &var4;
    var4[0][1][1]   = &var5;
    var5            = &var6;
    var6            = &var7;
    return 0;
}