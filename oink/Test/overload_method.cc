// test overloading
struct A {
  int f() {
    int
//        $tainted
      x;
    return x;
  }
  int f(int) {
    int
      $tainted
      y;
    return y;
  }
};

int main() {
  A a;
  int
    $untainted
    z;
  int arg = 3;
  z = a.f(arg);
}
