typedef struct {
  int a;
} s0;

typedef struct {
  int a;
} *s1;

typedef struct {
  int a;
} volatile * const & s2;

int foo(s0) {}
