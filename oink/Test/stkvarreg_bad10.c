void register_var(int **);
void unregister_var(int **);
void register_array_var(int **, int);
void unregister_array_var(int **);

void foo() {
  int $!regme *v4[3];           // array
  register_var(v4);             // but is registered as a non-array
  unregister_array_var(v4);
}
