// t0588.cc
// more tests w/ default args

struct S {} *s;
struct S2 {} *s2;

template <class T>
int f1(T *t, int x = 5);

template <class T>
int f1(T *t, int x);

template <class T>
int f1(T *t, int x)
{
  return x + 7;
}

int foo()
{
  return f1(s) + f1(s2);
}



template <class T>
int f2(T *t, int x = 6)
{
  return x + 8;
}

int bar()
{
  return f2(s) + f2(s2);
}


// EOF
