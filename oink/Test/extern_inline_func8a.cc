extern int foo();

extern inline int foo()
{
}

int main()
{
  return foo();
}
