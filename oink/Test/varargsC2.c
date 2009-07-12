int foo(int a, ... );
int main() {
  int x;
  int $tainted y;
  int $untainted z;
  z = foo(x, y);
}
