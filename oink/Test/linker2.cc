// test the linker imitation; this is file 2

extern int x;                   // seems to get merged with the below definition of x
int x;
static int x_static;

int foo(int y, double z) {
  int w = 1;
  w++;
  return w;
}
static int foo_static(int y, double z) {
  int q = 2;
  q++;
  return q;
}

typedef int int_typedef;

class C {
  int privC1;
  public:
  int x;                        // test the namespace disambiguation
  int c1;
  int foo(int y, double z);     // test the namespace disambiguation
  int foo2(int y) {
    int r = 3;
    static int rr = 4;
    return r;
  }
  int foo2(int y, double z);    // test overloading
  class D {
    int privD1;
    public:
    int x;                        // test the namespace disambiguation
    int d1;
    int foo(int y, double z);     // test the namespace disambiguation
    int foo2(int y);
    int foo2(int y, double z);    // test overloading
  };
};
