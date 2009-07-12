int main() {
  int $tainted *i;
  int *i2;
  float *f;
  float $untainted *f2;
  void *v = i;
  f2 = v;
}
