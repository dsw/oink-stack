# 1 "warnings-noreturn.c"
# 1 "testharness.h" 1
extern int printf(const char * format, ...);
#pragma ccuredvararg("printf", printf(1))

extern void exit(int);

 



# 1 "warnings-noreturn.c" 2



void croak() __attribute__((noreturn));
void die() __attribute__((noreturn));


void terminate(int) __attribute__((noreturn));

void terminate(int frog)
{
  if (frog)
    croak();
  else
    die();
}


int main()
{
  { printf("Success\n"); exit(0); } ;
}
