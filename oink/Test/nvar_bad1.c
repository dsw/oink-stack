void f(int*, int **);

int main() {
  int $!noargandref *x2;
  f(x2, &x2);
}
