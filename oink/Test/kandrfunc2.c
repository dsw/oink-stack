// test functions returning functions
#include <stdio.h>

double f0(double z) {
  printf("f0 z:%g\n", z);
  return z+3;
}

//  double (*f(int x, int y))(double z) {
double (*f(x, y))(double)
  int x;
  int y;
{
  printf("f x: %d, y:%d\n", x, y);
  return f0;
}

double (*(*g(x, y, z))())(double)
  int x;
  int y;
  double z;
{
  printf("g x: %d, y:%d, z:%g\n", x, y, z);
  return f;
}

int main() {
  {
  double q = f(1, 2)(7);
  printf("q:%g\n", q);          // 10
  }
  {
    // wrong!  You can't pass "6" to a double K&R param, you have to
    // pass 6.0!
//    double q2 = g(4,5,6)(7,8)(9);
  double q2 = g(4,5,6.0)(7,8)(9);
  printf("q2:%g\n", q2);         // 12
  }
  return 0;
}
