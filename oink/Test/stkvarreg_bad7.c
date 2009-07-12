void register_var(int **);
void unregister_var(int **);

void foo() {
  int $!regme *v4;              // should be registered
  unregister_var(&v4);
  register_var(&v4);            // is, but after unregistered
}
