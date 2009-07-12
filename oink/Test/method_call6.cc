// Test virtual method call
class A {
  virtual                       // bad
    void f() {
  }
};

class B : public A {
};

class C : public B {
  void f() {
    C * $untainted c = this;
  }
};

int main() {
  // NOTE: this is supposed to be an A*, as this test is testing that
  // C::f() is called, despite it s being referred to as A::f(), due
  // to the virtual keyword.
  A * $tainted a0 = new C();
  a0->f();
}
