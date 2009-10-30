int main() {
  int x;
  x = 11;
  while (x > 0) {
    if (x == 1) {
      x = x - 1;
      continue;
    }
    x = x - 2;
  }
  return x;
}
