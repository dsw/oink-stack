// Test virtuality inherits; B::f() is made virtual by A::f() being
// marked virtual, which causes C::f() to be called, causing a
// failure; but this time we check that multiple subclasses are
// scanned.
class A {
  virtual                       // bad
    void f() {
  }
};

class B : public A {
  void f() {
  }
};

class C : public B {
  void f() {
  }
};

class D : public B {
  void f() {
    B * $untainted b = this;
  }
};

int main() {
  B * $tainted b0 = new B();
  b0->f();
}
