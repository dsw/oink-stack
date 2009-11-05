#ifndef VERIFY1_FOO_H
#define VERIFY1_FOO_H

struct Foo {
  int x;
};

struct Foo *new_Foo();
void free_Foo(struct Foo *foo);

int get_x(struct Foo *foo);
void set_x(struct Foo *foo, int x);

#endif // VERIFY1_FOO_H
