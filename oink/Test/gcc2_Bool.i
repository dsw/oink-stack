// gcc 2.96 does not seem to define _Bool

typedef enum
  {
    false = 0,
    true = 1
  } _Bool;

_Bool x;
