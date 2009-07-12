// test overloading
struct A {
  A() {}
  A(int x) {
    int $untainted z;
    z = x;
  }
};

struct B {
  A a;
  B(int x) : a(x) {
  }
};

int main() {
  int $tainted arg = 3;
  B b(arg);
}
