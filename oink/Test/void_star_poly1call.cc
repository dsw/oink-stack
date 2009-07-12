void *f(void *p) {
  return p;
}

int main() {
  int                           // good
  float                         // bad
    $tainted * x;
  float $untainted * y;
  y = f(x);
}
