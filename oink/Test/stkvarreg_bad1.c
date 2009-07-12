void register_var(int **);

void foo() {
  int $!regme *v4;              // this one must be registered
  // and is not
}
