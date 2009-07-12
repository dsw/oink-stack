int f(int x) {
  return x;                     // bad
}

int main() {
  int dummy;
  int dummy2;
  int $tainted t;
  int $untainted u;
  // flow only in the non-polymorphic case
  dummy = f(t);
  u = f(dummy2);
}
