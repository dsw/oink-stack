// ERR-MATCH: Over-satisfied symbol 'fabsf'

extern int foo(void);

inline int foo(void)
{
  return 42;
}

int main()
{
  return foo();
}
