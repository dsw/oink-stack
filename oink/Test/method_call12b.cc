// more tests of the linker imitator; This is file 1.

// this would come from the common header file
struct A {
  virtual void f(int x);
  // replace with this to test a user error; I didn't think it was
  // worth it to automate
//    virtual void f();
};

// forward declaration from other file
void g(A *a);

// **** end of shared code

// this occurs in one file but not the other
struct B : A {
  /*virtual*/ void f(int x) {
    int $untainted y;
    y = x;
  }
};

int main() {
  B *b = new B;
  g(b);
}
