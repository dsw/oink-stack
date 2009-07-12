char *foo (const char *s)
{
  return (char *) s;
}

int main()
{
  const char *str;
  const char *p;

  p = foo (str);

  p = str;
}
