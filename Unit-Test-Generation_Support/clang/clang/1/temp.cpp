union UA{
     int a;
     double g;
};

typedef union UA  EA;
typedef int IA;
typedef IA IB;


int f1() {
    UA eb; 
    EA a;
    IA b;
    return 1;
}