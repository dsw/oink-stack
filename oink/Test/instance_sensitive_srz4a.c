extern void foo(char * p);

int main()
{
  char buf[1024];
  foo(buf);

  char $untainted * p = buf;
}
