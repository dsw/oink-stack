// member init ctors

struct A {
  A ();
  A (int) {
    // make "this" tainted
    A $tainted *a2 = this;
  }
};

struct B : A {
  B()
    : A(3)                      // bad
  {}
};

int main() {
  B b;
  A $untainted &a = b;
}
