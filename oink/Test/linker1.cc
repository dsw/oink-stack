// test the linker imitation; this is file 1

extern int x;
static int x_static;

int foo(int y, double z);
static int foo_static(int y, double z) {}

typedef int int_typedef;

class C {
  int privC1;
  public:
  int x;                        // test the namespace disambiguation
  int c1;
  int foo(int y, double z);     // test the namespace disambiguation
  int foo2(int y);
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

void red() {
  class E {
    void blue() {               // not visible to the linker
    }
  };
}
