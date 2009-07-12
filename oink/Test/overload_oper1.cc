// test operator overloading
//  #include <stdio.h>

struct A {
  int x;
  A(int x0) : x(x0) {}
};

A operator +(A $untainted a0, A a1) {
  return A(a0.x + a1.x);
}

int main() {
  A $tainted x(2);
  A y(3);
//    A z = operator + (x, y);
  A z = x + y;
//    printf("%d + %d = %d\n", x.x, y.x, z.x);
}
