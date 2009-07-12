// check what happens when you call a function with more args than it has
int f(int x, int y, ...) {      // make sure it is defined so don't get leaf poly
}

int main() {
  int x0;
  int y0;
  int z0;
  int a;
  int $tainted b;
  f(x0, y0, z0, a, b);             // "..." above should get tainted
}
