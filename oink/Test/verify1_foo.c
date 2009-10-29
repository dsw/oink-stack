#include <stdlib.h>

#include "verify1_foo.h"

struct Foo *new_Foo() {
  return (struct Foo*) malloc(sizeof(struct Foo));
}

int get_x(struct Foo *foo) {
  return foo->x;
}

void set_x(struct Foo *foo, int x) {
  foo->x = x;
}
