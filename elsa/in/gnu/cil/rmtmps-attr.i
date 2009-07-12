# 1 "rmtmps-attr.c"
# 1 "testharness.h" 1
extern int printf(const char * format, ...);
#pragma ccuredvararg("printf", printf(1))

extern void exit(int);

 



# 1 "rmtmps-attr.c" 2


int main()
{
  int a;
  int b __attribute__((myattribute(a == a)));
  b = 5;
   
   

   
   
  { printf("Success\n"); exit(0); } ; 
} 
