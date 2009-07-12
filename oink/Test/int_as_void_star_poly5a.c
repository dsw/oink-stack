// check that int-as-void* works across serialization

void f(int x) {
  float $untainted * fp = (float*) x;
}
