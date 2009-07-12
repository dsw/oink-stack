// Make sure that we consider a function called if its address is taken.
//
// libc annotation assumes this, so if this ever changes then glibc must be
// updated.


int foo()
{
  int $tainted t;
  int $untainted u;
  u = t;
}

int bar()
{
  &foo;
}
