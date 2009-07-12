int f(int q);

int main() {
  int $tainted x;
  int $untainted y;
  y = f(x);
}
