// possible ambiguity in parameters

typedef int x;

int foo ( int  x  )
{ }

int takesOneInt(int y) { return 6; }

int bar()
{
  int i, j;

  i = foo(4);
  j = foo(takesOneInt);

  return j;
}



