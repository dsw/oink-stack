void wow(int *x0);
int baz() {
  int z;
  int *x = &z;
  wow(x);
}
