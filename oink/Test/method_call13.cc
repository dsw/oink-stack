// check out method calls from within ctors
struct A {
  void g(int, int) {
//      A * $untainted a1 = this;
  }
  void f(int);
  ~A();
  A(double);
};

void A::f(int) {
  g(1,2);
}

A::~A() {
  g(3,4);
}

A::A(double) {
  g(5,6);
}

int main() {
//    A $tainted a(3.2);
}
