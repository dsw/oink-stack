// library for fooing

// #include <stdio.h>

void foo_read(int *y) {
  int q = *y;
  printf("foo_read: z is %d\n", q);
}

void foo_write(int *y) {
  *y = 4;
}
