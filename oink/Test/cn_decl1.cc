// test if CN_decl-s call their ctor
struct A {
  A(int y) {
    int $untainted z = y;
  }
  operator bool () { return true; }
};

int main() {
  int $tainted x;
  if (A a = x) {}               // bad
}
