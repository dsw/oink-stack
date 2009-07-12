// works
int gronk(int y, ...) {
  return y;
}

int main() {
  int $tainted x;

  // this way fails, since there is nothing to attach the parameters to
//    int (*pgronk)();

  int (*pgronk)(int, ...);      // this should work with a warning

  pgronk = &gronk;
  int y = pgronk(x);
}
