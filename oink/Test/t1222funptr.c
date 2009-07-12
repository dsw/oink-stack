// test function pointers work
typedef int (int_to_int_func)(int);
int f(int) {int $tainted x; return x;}
int main() {
  int $untainted y;
  int_to_int_func *g;
  g = f;
  y = g(1);                     // bad
}
