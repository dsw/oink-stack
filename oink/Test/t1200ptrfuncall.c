// works
int gronk(int y) {
  return y;
}

int main() {
  int $tainted x;
  int (*pgronk)(int);           // this should work
  pgronk = &gronk;
  int y = pgronk(x);            // good
  int $untainted y = pgronk(x); // bad
}
