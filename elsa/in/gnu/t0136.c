// gnu/t0136.c
// transparent union, with the attribute directly on the union

typedef struct S {
  int blah;
} S;

union __attribute__((transparent_union)) U {
  int *i;
  S *s;
};

int f(union U u)
{
  return *u.i;
}

int pass_int(int *p)
{
  return f(p /*implicitly initializes U.i*/);
}
