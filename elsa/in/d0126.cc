// #include "stdio.h"

// dsw: I am amazed that this works in both gcc and elsa

typedef void f_t(void);

f_t a;

struct A {
  f_t b;
};

void a(void) {
//   printf("a here\n");
}

void A::b(void) {
//   printf("A::b here\n");
}

int main() {
  A a2;
  a();
  a2.b();
  return 0;
}
