// this funky qualifier causes flow from the arugument to the return
int $_1_2 f(int $_1 x);         // bad

int f(int x) {
  // no flow
}

int main() {
  int $tainted t;
  int $untainted u;
  u = f(t);
}
