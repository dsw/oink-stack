void f(int *, int **);

int g(int *);
int g2(int **);

int main() {
  int *x;
  int $!noargandref *x2;
  int **y;
  f(x2, y);
  f( g(x2), y );
  f(x, &x2);
  f( g2(&x2), x );
  f( x2 = 3, x );
}
