int f(int p) {
  return p;
}

int main() {
  int                           // good
  float                         // bad
    $tainted * x;
  float $untainted * y;
  y = (float*) f( (int) x);
}
