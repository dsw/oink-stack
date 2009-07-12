void register_var(int **);
void unregister_var(int **);

void foo() {
  int $!regme *v4;
  if (v4) return 4;             // used before registered
  register_var(&v4);
  v4;
  unregister_var(&v4);
}
