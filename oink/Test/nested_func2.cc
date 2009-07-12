// test calling nested functions works as expected

int f() {
  int $tainted x;
  return x;
}

int main() {
  int f() {                     // good
    int x;                      // good
    return x;                   // good
  }                             // good
  int $untainted y;
  y = f();
}
