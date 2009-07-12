// gnu/t0133.c
// transparent union

typedef struct S {
  int blah;
} S;

typedef struct T {
  int gorf;
} T;

typedef union W {
  T *t;
} W;  // __attribute__((transparent_union));

typedef union U {
  W w;
  int *i;
  S *s;
} U __attribute__((transparent_union));

int f(U u)
{
  return *u.i;
}

int pass_int(int *p)
{
  return f(p /*implicitly initializes U.i*/);
}

// normalizes to:
int pass_int_normalized(int *p)
{
  return f( (U){ .i = p } );
}

int pass_S(S *s)
{
  return f(s /*implicitly initializes U.s*/);
}

int pass_S_normalized(S *s)
{
  return f( (U){ .s = s } );
}

// This is *not* accepted by gcc-3.4.3, even if we
// mark W as being a transparent union also.
#if 0
int pass_T(T *t)
{
  return f(t /*implicitly initializes U.w.t*/);
}
#endif // 0

