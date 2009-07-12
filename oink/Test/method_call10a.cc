// Test virtual method call across translation units.  A multi-file test.
class A {
  virtual void f();
};

void A::f() {}

class B : public A {};

// a red herring
class D : public B {};

A *make_an_A();

int main() {
  A * $tainted a0 = make_an_A();
  a0->f();
}
