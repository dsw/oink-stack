void f(int x, int $!argstackparam *y2, int **z);

int main() {
  int x;
  int *y;
  int $!argstackparam *y2;
  int **z;
  f(x,
    y2+0                        // not a variable
    , z);
}
