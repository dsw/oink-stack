// check that int-as-void* works across serialization

void f(int x);

int main() {
  float $tainted * fp0;
  f((int)fp0);
}
