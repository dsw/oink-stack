#include <assert.h>

#include "verify1_foo.h"

int main() {
  struct Foo *foo = new_Foo();
  set_x(foo, 1);
  int x = get_x(foo);
  assert(x == 1);
  return 0;
}
