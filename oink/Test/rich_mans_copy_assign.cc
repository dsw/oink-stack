// this should pass if we handle "this" specially; testing default
// copy assign

struct A {
  int q;
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

  a2 ->operator= (*a1);         // bad
  *a3 = *a2;
  *a4 = *a3;

  a1->q = t;
  u = a4.q;

  return 0;
}
