#include <stdio.h>

void f(int a[]) {
}

int main() {
  // from Derrick
  int a[] = { 0, 1, 2 };
  char str[] = "Hello world!";
  printf(str);
  f(a);
  return 0;
}
