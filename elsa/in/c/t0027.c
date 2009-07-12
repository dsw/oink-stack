/* t0027.c
 * pass structure by value */

typedef struct S {
  int x;
  int y;
} S;

int f(S s)
{
  return s.x + s.y;
}

int g()
{
  S s;
  s.x = 5;
  s.y = 7;
  return f(s);
}

/* EOF */
