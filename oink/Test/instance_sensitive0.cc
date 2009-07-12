// this test applies with and without instance sensitivity

struct A {
  int q;
  int r;
};

int main() {
  struct A *a1;
  struct A *a2;
  struct A *a3;
  int $tainted x;
  int $untainted y;
  a1->q = x;
  a2 = a1;
  a3 = a2;
  y = a3->
    q                           // bad
    r                           // good
    ;
}
