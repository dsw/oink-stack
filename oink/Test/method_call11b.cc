// Test virtual method call across translation units.  A multi-file test.
class A {
  void f() {
    A * $untainted c = this;
  }
};

int main() {
  A * $tainted a0 = new A();
  a0->f();
}
