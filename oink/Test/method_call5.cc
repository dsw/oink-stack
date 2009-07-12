// Test virtual method call
class A {
  virtual void f() {
    A * $untainted a = this;    // good
  }
};

class B : public A {
  void f() {
    B * $untainted b = this;    // bad
  }
};

int main() {
  // NOTE: this is supposed to be a B* not an A* !  I am testing that
  // B::f() gets called; other tests test that virtuality works right;
  // this one just tests that it doesn't interfere.
  B * $tainted b0 = new B();
  b0->f();
}
