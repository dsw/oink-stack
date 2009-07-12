// this is part one of a multiple file input
static int gronk(int x);
int main() {
  int $tainted x;
  int $untainted y;
  y = gronk(x);                 // bad
}
int gronk(int x) {
  return 1;
}
