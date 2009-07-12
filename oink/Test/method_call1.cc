// Test making a method call
class A {
  void f(int x) {
    A * $untainted a1 = this;
  }
};

int main() {
  A * $tainted a0 = new A();
  int x0;
  a0->f(x0);
}
