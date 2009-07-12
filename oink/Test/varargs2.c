// test real varargs
#include <stdarg.h>
#include <stdio.h>

int oink(int num, ...) {
  int last;
  va_list l;
  va_start(l, num);
  for (int i=0; i<num; ++i) {
//      char *s;
//      s = va_arg(l, char *);
//      printf("%d, %s\n", i, s);
    int s;
    s = va_arg(l, int);
    printf("%d, %d\n", i, s);
    last = s;
  }
  va_end(l);
  return last;
}

int main() {
//    oink(3, "I say hello", "you say goodbye", "I don't know why you say goodbye I say hello");
  int $tainted x;
  x = 82;
  int y;                        // should get tainted
  y = oink(3, 4, 17, x);
  printf("last: %d\n", y);
}
