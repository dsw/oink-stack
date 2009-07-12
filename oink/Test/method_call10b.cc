// Test virtual method call across translation units.  A multi-file test.
class A {
  virtual void f();
};

class B : public A {};

class C : public B {
  void f() {
    C * $untainted c = this;
  }
};

A *make_an_A() {
  return new C();
}
