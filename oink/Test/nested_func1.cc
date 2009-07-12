// test calling nested functions works as expected

int f() {
  int x;
  return x;
}

int main() {
  int f() {                     // bad
    int $tainted x;             // bad
    return x;                   // bad
  }                             // bad
  int $untainted y;
  y = f();
}
