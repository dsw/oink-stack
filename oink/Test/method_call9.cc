// Test virtual method call
class A {
  virtual                       // bad
    void f() {
  }
};

class B : public A {
  void f();
};

void B::f() {
  B * $untainted b = this;
}

int main() {
  A * $tainted a0 = new B();
  a0->f();
}
