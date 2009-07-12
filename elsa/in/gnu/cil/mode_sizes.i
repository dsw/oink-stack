# 1 "mode_sizes.c"
 
 

 
typedef int int8_t __attribute__ ((__mode__ (  __QI__ ))) ;
typedef int int16_t __attribute__ ((__mode__ (  __HI__ ))) ;
typedef int int32_t __attribute__ ((__mode__ (  __SI__ ))) ;
typedef int int64_t __attribute__ ((__mode__ (  __DI__ ))) ;

typedef unsigned int u_int8_t __attribute__ ((__mode__ (  __QI__ ))) ;
typedef unsigned int u_int16_t __attribute__ ((__mode__ (  __HI__ ))) ;
typedef unsigned int u_int32_t __attribute__ ((__mode__ (  __SI__ ))) ;
typedef unsigned int u_int64_t __attribute__ ((__mode__ (  __DI__ ))) ;

typedef int someInt;       

 
someInt printf(char const *fmt, ...);

int main()
{
  int ok = 1;

  


  printf("size of " "int8_t" " is: %d (should be %d)\n", sizeof( int8_t ),   1 ); ok = ok && (sizeof( int8_t ) ==   1 ) ;
  printf("size of " "int16_t" " is: %d (should be %d)\n", sizeof( int16_t ),   2 ); ok = ok && (sizeof( int16_t ) ==   2 ) ;
  printf("size of " "int32_t" " is: %d (should be %d)\n", sizeof( int32_t ),   4 ); ok = ok && (sizeof( int32_t ) ==   4 ) ;
  printf("size of " "int64_t" " is: %d (should be %d)\n", sizeof( int64_t ),   8 ); ok = ok && (sizeof( int64_t ) ==   8 ) ;

  printf("size of " "u_int8_t" " is: %d (should be %d)\n", sizeof( u_int8_t ),   1 ); ok = ok && (sizeof( u_int8_t ) ==   1 ) ;
  printf("size of " "u_int16_t" " is: %d (should be %d)\n", sizeof( u_int16_t ),   2 ); ok = ok && (sizeof( u_int16_t ) ==   2 ) ;
  printf("size of " "u_int32_t" " is: %d (should be %d)\n", sizeof( u_int32_t ),   4 ); ok = ok && (sizeof( u_int32_t ) ==   4 ) ;
  printf("size of " "u_int64_t" " is: %d (should be %d)\n", sizeof( u_int64_t ),   8 ); ok = ok && (sizeof( u_int64_t ) ==   8 ) ;
  

  return ok? 0 : 1;
}
