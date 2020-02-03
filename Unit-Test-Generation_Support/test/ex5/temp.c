// Testing anonimous nested structures

struct {
    struct {
        int f1;
        char f2;
    }va;
}vb;


int f1(){
  vb.va.f1 = 111;
  return 0;
}