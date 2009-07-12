// works
int $_1 gronk(int godzilla, int $_1 oink);

int main() {
  int a;
  int $tainted x;

  int (*pgronk)(...);           // info might be lost here
  pgronk = &gronk;

  int (*pgronk2)(...);
  pgronk2 = pgronk;

  int y = pgronk2(a, x);        // good
  int $tainted y = pgronk2(a, x); // bad
}
