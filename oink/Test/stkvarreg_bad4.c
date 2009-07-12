void register_var(int **);
void unregister_var(int **);

void foo() {
  int $!regme *v4;
  if (3) return 4;
  register_var(&v4);
  unregister_var(&v4);
  if (1) {
    while (1) {
      v4;                       // used after unregistered
    }
  }
}
