// heap ctors

struct A {
  A(int $untainted x);
};

int main() {
  int $tainted x;
  A *a
    = new A(x)                  // bad
    ;
}
