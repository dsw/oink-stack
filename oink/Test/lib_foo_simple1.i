# 248 "/usr/include/stdio.h" 3 4
int printf(const char * , ...) ;

# 1 "hello.h" 1
extern int z;
# 6 "lib_foo.c" 2



void foo_read(int *y) {
  int q = *y;
  printf("foo_read: z is %d\n", q);
}

void foo_write(int *y) {
  *y = 4;
}

void read_z() {
  int r = z;
  printf("read_z: z is %d\n", r);
}
