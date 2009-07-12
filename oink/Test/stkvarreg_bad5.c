void register_var(int **);

void foo() {
  int $!regme **v4;             // shouldn't be registered (too many *-s)
  register_var(&v4);            // but is
}
