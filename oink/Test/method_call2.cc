// Test making a method call
class A {
  void f(int x) {
    A * $untainted a1 = this;
  }
};

A * make_A() {
  A * $tainted a0 = new A();
  return a0;
}

int main() {
  int x0;
  make_A()->f(x0);
}
