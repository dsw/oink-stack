// check that int-as-void* works across serialization

void f(char q, ...);

int main() {
  int * x;
  float $tainted * y;
  double * z;
  f('a', x, y, z);
}
