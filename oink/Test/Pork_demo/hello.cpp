#include <stdio.h>

int volatile x = 0;

class Foo {
public:
  void waga() {
    printf("waga\n");
    for (int i=0; i<100; ++i) {
      for (int j=0; j<100; ++j) {
        ++x;
      }
      putchar('.');
    }
    printf("\n");
  }
};

void zap() {
  printf("zap\n");
  for (int i=0; i<50; ++i) {
    for (int j=0; j<50; ++j) {
      ++x;
    }
    putchar('.');
  }
  printf("\n");
}

int main() {
  Foo f;
  f.waga();
  zap();
  f.waga();
  return 0;
}
