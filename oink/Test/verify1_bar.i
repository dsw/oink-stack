# 1 "Test/verify1_bar.c"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "Test/verify1_bar.c"
# 1 "/usr/include/assert.h" 1 3 4
# 42 "/usr/include/assert.h" 3 4
# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 43 "/usr/include/assert.h" 2 3 4
# 75 "/usr/include/assert.h" 3 4

void __assert_rtn(const char *, const char *, int, const char *) __attribute__((__noreturn__));
void __eprintf(const char *, const char *, unsigned, const char *) __attribute__((__noreturn__));

# 2 "Test/verify1_bar.c" 2

# 1 "Test/verify1_foo.h" 1



struct Foo {
  int x;
};

struct Foo *new_Foo();
void free_Foo(struct Foo *foo);

int get_x(struct Foo *foo);
void set_x(struct Foo *foo, int x);
# 4 "Test/verify1_bar.c" 2

int main() {
  struct Foo *foo = new_Foo();
  set_x(foo, 1);
  int x = get_x(foo);
  (__builtin_expect(!(x == 1), 0) ? __assert_rtn(__func__, "Test/verify1_bar.c", 9, "x == 1") : (void)0);
  free_Foo(foo);
  return 0;
}
