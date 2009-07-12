// works
int $_1 gronk(int $_1 oink);

int main() {
  int $tainted x;
  int (*pgronk)(int);           // this should work
  pgronk = &gronk;
  int y = pgronk(x);            // good
  int $untainted y = pgronk(x); // bad
}
