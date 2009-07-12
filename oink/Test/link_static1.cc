// foo is not defined at link time.  This should give a link error.

static int foo();

int main()
{
  foo();
}
