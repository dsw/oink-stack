void register_var(int **);

void foo() {
  int $!regme *v4;              // this one must be un-registered
  if (3) return 4;
  register_var(&v4);
  // and is not
}
