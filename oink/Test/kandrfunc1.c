// page 218 K&R2nd.
int f(), *fpi(), (*pfi)();

int gronk(x, y)
  register int x;               // register allowed
// I don't think this volatile should be allowed
//    volatile int y;               // no other "storage class specifier" allowed, K&R2nd 226
  int y;                        // no other "storage class specifier" allowed, K&R2nd 226
{
  return 7;
}

int gronkB(x, y)
  int x, y;
{
  return 7;
}

int *gronk2(x, y)
  int x;
  int y;
{
}

int *(gronk3(x, y))
  int x;
  int y;
{
  int *q = 0;
  return q;
}

int (*oink1())(int)
{
  return gronk;
}

int (*oink2(x))(int)
    int x;                        // ignored
{
  return gronk;
}

int (*oink3(x))()
    int x;                        // ignored
{
  return gronk;
}

