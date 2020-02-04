// ex6
// Testing anonimous nested structures
// The nested structure doesn't declare a field

struct {
    struct {
        int f1;
        char f2;
    };
}vb;
struct x1{
    struct x2{
        int f1;
        char f2;
    }fx2;
}xvb;


int f1(){
  vb.f1 = 111;
  xvb.fx2.f1 = 112;
  return 0;
}