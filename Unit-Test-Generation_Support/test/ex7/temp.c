// ex7
// The dreadful case 7, this is mostly to see if it "runs"
// not really to compare the variables values.
// Nested types on variable declaration


int  ***(*(((*(*var1[5]))[4])[3])[2])[1][2][3];
int  ***(*(((*  var2    )[4])[3])[2])[1][2][3];
int  ***(*((    var3     [4])[3])[2])[1][2][3];
int (***(*((     var3B   [4])[3])[2])[1][2][3])(int);
int  ***        var4                 [1][2][3];
int  **         var5                          ;
int  *          var6                          ;
int             var7                          ;

int f1(){
    var1[1]         = &var2;
    var2            = &var3;
    var3[1][1][1]   = &var4;
    var4[0][1][1]   = &var5;
    var5            = &var6;
    var6            = &var7;
            return 0;
}