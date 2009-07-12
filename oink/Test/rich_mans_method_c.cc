// this should pass if we handle "this" specially; testing method calls

struct A {
  int q;
  int &get_q() { return &q; }
};

int main() {
  int $tainted a;
  int a2;
  A *x1;
  A *x2;

  x1->get_q() = a;
  x2->get_q() = a2;
  
  int $untainted z =
    x1                          // bad
    x2                          // good
    .get_q();
}
