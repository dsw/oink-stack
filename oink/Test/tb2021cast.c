// test that casting works
int main() {
  int x;
  x = ($tainted int) 3;
  int $untainted y;
  y = x;
}
