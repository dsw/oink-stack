// test whether or not the linker can deal with a struct and a
// function of the same name being in the same namespace without
// colliding them; the struct should not show up as a linker-visible
// name

struct foo {
  int x;
};

void foo();
