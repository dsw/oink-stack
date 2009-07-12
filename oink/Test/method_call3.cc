// Test making a static method call
class A {
  static void f(A * x) {
    A * $untainted a1 = x;
  }
};

int main() {
  A * a0 = new A();
  A * $tainted x0;
  a0->f(x0);
}
