#include <stdio.h>

void f() {
  printf("f\n");
}

int main() {
  printf("main\n");
  f();
  f();
  return 0;
}
