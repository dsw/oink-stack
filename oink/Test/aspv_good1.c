void g(int $!argstackparam *y2);

void f(int x, int $!argstackparam *y2, int **z) {
  g(y2);
}

int *y;                         // global, but not of value '$!argstackparam *'

int main() {
  int x;
  int $!argstackparam *y2;
  int **z;
  f(x, y, z);
  f(x, y2, z);
}
