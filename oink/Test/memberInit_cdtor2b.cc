// member init ctors

struct A {
  A () {
    // make "this" tainted
    A $tainted *a2 = this;
  }
  A (int);
};

struct B : A {
  B()
    : A(3)                      // good
  {}
};

int main() {
  B b;
  A $untainted &a = b;
}
