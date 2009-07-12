// immediate temporary dtors

struct A {
  A() {}
  ~A() {
    A $untainted *a = this;     // bad
  }
};

int main() {
  A $tainted &a = A();          // dtored at close curly
}
