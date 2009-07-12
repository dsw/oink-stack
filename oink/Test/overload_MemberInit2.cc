// test overloading
struct A {
  A() {}
  A(int x) {
    int $untainted z;
    z = x;
  }
};

struct B : A {
  B(int x) : A(x) {
  }
};

int main() {
  int $tainted arg = 3;
  B b(arg);
}
