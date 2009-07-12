// test operator overloading
//  #include <stdio.h>

struct A {
  int x;
  A(int x0) : x(x0) {}
  A operator +(A $untainted a) {
    return A(x + a.x);
  }
};

int main() {
  A x(2);
  A $tainted y(4);
//    A z = operator + (x, y);
  A z = x + y;
//    printf("%d + %d = %d\n", x.x, y.x, z.x);
}
