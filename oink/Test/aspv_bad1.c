void f(int x, int $!argstackparam *y2, int **z);

int $!argstackparam *y2;        // global

int main() {
  int x;
  int *y;
  int **z;
  f(x, y2, z);
}
