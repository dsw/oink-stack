# 1 "typeof1.c"
 

extern int printf(const char* fmt, ...);


void foo();


__typeof(foo) afun;  
void afun() {}

void bfun(void);  
extern __typeof(afun) bfun __attribute__ ((alias ("afun")));  

int arr[9];

__typeof(arr) barr = { 0, 1, 2, 3 } ;


__typeof("a long string") str;  




typedef int FUN(int);

FUN fptr;  

FUN fptr;  

int fptr(int x);  

int fptr(int x) {  
  return x - 1;
}

typedef int ARRAY[8];

ARRAY carr;

int main(void) 
{

  afun();
  bfun();
   







  { char a[] = { [ sizeof(foo) ] = 34 }; printf("sizeof(foo)" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( sizeof(foo) )); if( sizeof(foo)  != sizeof(a) - 1) { exit(1); } } ;
  { char a[] = { [ sizeof(afun) ] = 34 }; printf("sizeof(afun)" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( sizeof(afun) )); if( sizeof(afun)  != sizeof(a) - 1) { exit(1); } } ;
  { char a[] = { [ sizeof("a long string") ] = 34 }; printf("sizeof(\"a long string\")" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( sizeof("a long string") )); if( sizeof("a long string")  != sizeof(a) - 1) { exit(1); } } ;
  { char a[] = { [ sizeof(str) ] = 34 }; printf("sizeof(str)" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( sizeof(str) )); if( sizeof(str)  != sizeof(a) - 1) { exit(1); } } ;
  { char a[] = { [ sizeof(arr) ] = 34 }; printf("sizeof(arr)" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( sizeof(arr) )); if( sizeof(arr)  != sizeof(a) - 1) { exit(1); } } ;
  { char a[] = { [ sizeof(barr) ] = 34 }; printf("sizeof(barr)" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( sizeof(barr) )); if( sizeof(barr)  != sizeof(a) - 1) { exit(1); } } ;

  { char a[] = { [ __alignof("a string") ] = 34 }; printf("__alignof(\"a string\")" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( __alignof("a string") )); if( __alignof("a string")  != sizeof(a) - 1) { exit(1); } } ;
  { char a[] = { [ __alignof(str) ] = 34 }; printf("__alignof(str)" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( __alignof(str) )); if( __alignof(str)  != sizeof(a) - 1) { exit(1); } } ;
  { char a[] = { [ __alignof(foo) ] = 34 }; printf("__alignof(foo)" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( __alignof(foo) )); if( __alignof(foo)  != sizeof(a) - 1) { exit(1); } } ;
  { char a[] = { [ __alignof(afun) ] = 34 }; printf("__alignof(afun)" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( __alignof(afun) )); if( __alignof(afun)  != sizeof(a) - 1) { exit(1); } } ;
  { char a[] = { [ __alignof(arr) ] = 34 }; printf("__alignof(arr)" " = %d (CIL) and %d (Compiler)\n", sizeof(a) - 1, ( __alignof(arr) )); if( __alignof(arr)  != sizeof(a) - 1) { exit(1); } } ;

   
   
   


  if(sizeof(carr) != sizeof(ARRAY)) {
    exit(8);
  }
  return fptr(1);
}
