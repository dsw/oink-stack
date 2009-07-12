# 1 "align1.c"
# 1 "testharness.h" 1
extern int printf(const char * format, ...);
#pragma ccuredvararg("printf", printf(1))

extern void exit(int);

 



# 1 "align1.c" 2



struct testalign {
  int f1;
} __attribute__((__aligned__(16)));

struct t1 {
  int f0;
  struct testalign a;
};

int main() {
  int offset;

  offset = &((struct t1*)0)->a.f1;
  printf("Offset is: %d\n", offset);

  if ((int)&(( struct t1  *)0)->a.f1 & 15) {
    { printf("Error %d\n",  1 ); exit( 1 ); } ;
  }


  { printf("Success\n"); exit(0); } ;
}
