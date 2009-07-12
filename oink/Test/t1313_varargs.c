// too few args
int f(int x, ...);
int main() {
  f();                          // bad
  f(1, 2);                      // good
}
