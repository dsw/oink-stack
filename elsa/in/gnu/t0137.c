// gnu/t0137.c
// transparent union: does the transparency flow down to the union
// itself when applied to a typedef?  no
//
// fuck that.  it will in Elsa anyway.

union U {
  int *i;
};

typedef union U U2 __attribute__((transparent_union));

int f(union U u)
{
  return *u.i;
}

void pass_int(int *p)
{
  // not accepted by GCC
  //ERROR(1): f(p /*implicitly initializes U.i*/);
}
