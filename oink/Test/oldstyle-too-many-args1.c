// ERR-MATCH: c77e9b0c-8b23-46f4-b0cc-64da19719a05
// ERR-MATCH: Function call has more arguments than the function has parameters

int foo(a) int a;
{
}

int main()
{
  foo(0, 0);
}
