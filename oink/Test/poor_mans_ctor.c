// this is a C version of what happens when a ctor is called.  Rob
// says that polymorphism won't work right

// this should fail

struct A {
  int q;
};

A x1;
A x2;

void A_ctor(A *thiz, int y) {
  thiz->q = y;
}

int main() {
  int $tainted a;
  int a2;
  A_ctor(&x1, a);
  A_ctor(&x2, a2);
  int $untainted z = x2.q;
}
