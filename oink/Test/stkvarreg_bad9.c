void register_var(int **);
void unregister_var(int **);

void foo() {
  int $!regme *v4;              // should be unregistered
  register_var(&v4);
  unregister_var(&v4);          // is
  unregister_var(&v4);          // but twice
}
