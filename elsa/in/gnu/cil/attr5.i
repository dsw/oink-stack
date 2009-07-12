# 1 "attr5.c"
# 1 "testharness.h" 1
extern int printf(const char * format, ...);
#pragma ccuredvararg("printf", printf(1))

extern void exit(int);

 



# 1 "attr5.c" 2


int x;
int * myfunc(void) __attribute__((section(".modinfo")));
int * myfunc(void) {
  return &x;
}

int main() {
  if(&x != myfunc()) { printf("Error %d\n",  1 ); exit( 1 ); } ;
  
  { printf("Success\n"); exit(0); } ;
}
