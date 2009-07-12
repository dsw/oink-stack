int $_1_2 foo(int a, ... $_1 );
int main() {
  int x;
  int $tainted y;
  int $untainted z;
  z = foo(x, y);
}
