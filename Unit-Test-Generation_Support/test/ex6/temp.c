// Testing anonimous nested structures

struct {
    unsigned char a:1;
    unsigned char b:2;
    unsigned char c:1;
}vb;


int f1(){
  vb.a = 1;
  return 0;
}