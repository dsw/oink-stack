// immediate temporary ctors to refs

struct A {
  A () {
    // make "this" tainted
    A $tainted *a2 = this;
  }
  A (int) {}
};

int main() {
  A $untainted &a
    = A()                       // bad
    = A(3)                      // good
    ;
}
