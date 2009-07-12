void register_var(int **);
void unregister_var(int **);
void register_array_var(int **, int);
void unregister_array_var(int **);

void foo() {
  int $!regme *v4;              // non-array
  register_var(v4);
  unregister_array_var(v4);     // but is unregistered as a array
}
