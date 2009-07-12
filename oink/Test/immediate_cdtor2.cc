// immediate temporary ctors

struct A {
  A();
  A (int) {
    // make "this" tainted
    A $tainted *a2 = this;
  }
};

// since this reference parameter will be bound to a temporary,
// it must be a reference-to-const
void f(A const $untainted &);

int main() {
  f(A(3));                      // bad
  f(A());                       // good
}
