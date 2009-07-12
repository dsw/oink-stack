// too many args
int f();
int main() {
  f(1);                         // bad
  f();                          // good
}
