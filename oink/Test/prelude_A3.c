// this funky qualifier causes flow from the arugument to the return
int f(int $_1 x) {
  return x;                     // bad
}

int main() {
  int $tainted t;
  int $untainted u;
  u = f(t);
}
