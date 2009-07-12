void foo(int a, ... $tainted);
int main(void) {
  int x, y;
  char *z;
  foo(x, y, z);
}
