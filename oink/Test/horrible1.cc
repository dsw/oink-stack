// class template, reference member, pointer to virtual function
// member

template<class T> struct A {
  virtual T f(int x) {}
};

struct B : A<int> {
  int &y;
  B(int &y0) : y(y0) {}
  int f(int x) {
    y = x;
  }
};

int main() {
  int $untainted y;
  A<int> *a0 = new B(y);
  int (A<int>::*q)(int) = &A<int>::f;
  int $tainted x = 0;
  (a0->*q)(x);
}
