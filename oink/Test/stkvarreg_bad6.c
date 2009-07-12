void register_var(int **);
void unregister_var(int **);

void foo() {
  int $!regme **v4;             // shouldn't be unregistered (too many *-s)
  unregister_var(&v4);          // but is
}
