// too many args
int f(void);
int main() {
  f();                          // good
  f(1);                         // bad
}
