void register_var(int **);
void unregister_var(int **);

void foo() {
  int $!regme *v4;              // should be registered
  register_var(&v4);            // is
  register_var(&v4);            // but twice
  unregister_var(&v4);
}
