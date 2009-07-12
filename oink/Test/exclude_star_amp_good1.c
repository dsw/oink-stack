int main() {
  int *x;
  *x = 3;
  int **y = &x;
  int *z = *y;
}
