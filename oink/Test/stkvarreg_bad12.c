void register_var(int **);
void unregister_var(int **);
void register_array_var(int **, int);
void unregister_array_var(int **);

void foo() {
  int $!regme *v4;              // non-array
  register_array_var(v4, 3);    // but is registered as an array
  unregister_var(v4);
}
