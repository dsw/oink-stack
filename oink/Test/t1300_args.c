// too few args
int f(int x, int y, int z);
int main() {
  f(1, 2);                      // bad
  f(1, 2, 3);                   // good
}
