// Test making a method call from another method (without an
// E_fieldAcc).
class A {
  void f() {
    g();                        // bad
  }
  void g() {
    A * $untainted a = this;
  }
};

int main() {
  A * $tainted a0 = new A();
  a0->f();
}
