#include <stdio.h>

void f(int a[]) {
}

void bar() {
  // from Derrick
  int a[] = { 0, 1, 2 };
  f(a);
}

void foo() {
  // from Derrick
  char str[] = "Hello world!";
  printf(str);
}

void g() {
  int a2[5] = {0};
  f(a2);
}
