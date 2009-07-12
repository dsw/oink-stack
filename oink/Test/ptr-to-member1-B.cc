// pointer to function member
class A {
  public:
  int f(int x) {
    int $untainted z = x;
  }
};

int main() {
  A *a0 = new A();
  int (A::*quint)(int);
  quint = &A::f;
  int $tainted y;
  (a0->*quint)(y);              // bad
}
