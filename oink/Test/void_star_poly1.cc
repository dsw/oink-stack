int main() {
  int                           // good
  float                         // bad
    $tainted * x;
  float $untainted * y;

  void * q;
  q = x;
  y = q;
}
