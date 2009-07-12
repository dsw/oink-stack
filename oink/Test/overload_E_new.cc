// test overloading
struct A {
  A() {}
  A(int x) {
    int $untainted z;
    z = x;
  }
};

int main() {
  int $tainted arg = 3;
  A *a;
  a = new A(arg);
}
