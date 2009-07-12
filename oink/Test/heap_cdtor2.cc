// heap ctors

struct A {
  A () {
    A $untainted *a2 = this;
  }
  A (int) {
    A *a3 = this;
  }
};

int main() {
  A $tainted *a
    = new A()                   // bad
    = new A(3)                  // good
    ;
}
