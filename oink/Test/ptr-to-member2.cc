// pointer to virtual function member
class A {
  public:
  virtual                       // bad
    int f(int x) {
  }
};

class B : public A {
  public:
  int f(int x) {
    int $untainted y = x;
  }
};

int main() {
  A *a0 = new B();
  int (A::*q)(int);
  q = &A::f;
  int $tainted x = 0;
  (a0->*q)(x);
}
