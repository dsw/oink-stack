// works, but doesn't test what I thought.  Only tests that the return
// values are getting hooked up.
int $_1_2 gronk(int godzilla, int $_1 oink);

int main() {
  int a;
  int $tainted x;

  int (*pgronk)(...);           // info might be lost here
  pgronk = &gronk;

  int (*pgronk2)(int, int);
  pgronk2 = pgronk;

  int y = pgronk2(a, x);        // good
  int $tainted y = pgronk2(a, x); // bad
}
