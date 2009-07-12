void f(int *, int **);

int g2(int **);

int main() {
  int $!noargandref *x2;
  f( g2(&x2), x2 );
}
