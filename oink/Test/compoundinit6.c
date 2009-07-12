// test range designator initializers
int main() {
  int x[] = { [1 ... 3] = 0, [5 ... 7] = 1};
  int y[] = { [1 ... 3] = 0, (int $tainted) 13, [5 ... 7] = 1};
  int z[] = { [1 ... 3] = (int $tainted) 0, [5 ... 7] = 1};
  int w[] = { [1 ... 3] = 0, [5 ... 7] = (int $tainted) 1};
}
