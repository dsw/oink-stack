
int bar(int x) __attribute__((alias("foo")));

int foo(int x) {
  return x;
}
