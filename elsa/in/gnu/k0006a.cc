// __builtin_offsetof

// #include <stddef.h>

struct S {
  int xxx;
  int yyy;
};

// int a = offsetof(S, yyy);

// gcc-4.0, gcc-4.1
int static_assertion1[ __builtin_offsetof(S,yyy) == 4 ? 1 : -1 ];

// gcc-3.4
int static_assertion2[ __offsetof__(((int) &((S *)0)->yyy)) == 4 ? 1: -1 ];

// gcc-3.3
int static_assertion3[ (((int) &((S *)0)->yyy) == 4) ? 1 : -1 ];

// int AAAAA = (int) &((S *)0)->yyy;
