int main() {
  int                           // good
  float                         // bad
    $tainted * x;
  float $untainted * y;

  int q;
  q = (int) x;
  y = (float*) q;
}
