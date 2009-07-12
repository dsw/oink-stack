// test overloading
struct A {
  static int f() {
    int
//        $tainted
      x;
    return x;
  }
  static int f(int) {
    int
      $tainted
      y;
    return y;
  }
};

int main() {
  int
    $untainted
    z;
  int arg = 3;
  z = A::f(arg);
}
