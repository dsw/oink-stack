// this should pass if we handle "this" specially; testing default
// copy assign

//  #include <iostream.h>

struct A {
  int q;
//    A(int q0) : q(q0) {}
  A &operator = (A const &other) {
    q = other.q;                // bad
//      cout << "copy assign operator here, q:" << q << endl;
  }
//    void dump() {
//      cout << "q: " << q << endl;
//    }
};

int main() {
  int $tainted t;
  int $untainted u;

//    A *a1 = new A(1);
//    A *a2 = new A(2);
//    A *a3 = new A(3);
//    A *a4 = new A(4);
  A *a1;
  A *a2;
  A *a3;
  A *a4;

//    a4->dump();

  a2 ->operator= (*a1);
  *a3 = *a2;
  *a4 = *a3;

//    a4->dump();

  a1->q = t;
  u = a4.q;

  return 0;
}
