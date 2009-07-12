int f(int x) {
  return x;
}

int main() {
  int $tainted x0;
  int y0;
  y0 = f(x0);

  int x1;
  int $untainted y1;
  y1 = f(x1);
}
