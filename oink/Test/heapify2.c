// #include <stdlib.h>

// fake
void longjmp();

int main() {
  int x;
  &x;
  int y = 200;
  &y;
  if (1) {
    int z;
    &z;
    while (1) {
      int a = 300;
      &a;
      return 1;                 // a, z, y, x
    }
    return 2;                   // z, y, x
  } else {
    int b;
    &b;
    return 3;                   // b, y, x
  }
  for(;;) {
    int c;
    int d;
    &c;
                                // c
  }
  for(;;) {
    int e;
                                // <nothing>
  }

 foo:
  goto foo;
  while(1) {
    break;
    continue;
  }
  longjmp();

  return 100;                   // y, x
}
