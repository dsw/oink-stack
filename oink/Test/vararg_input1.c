int printf(const char $untainted * fmt, ...) {}
char $tainted * getenv(const char* x) {}

int sprintf (char* buf, const char $untainted * format, ...)
{
  __builtin_va_list arg;
  __builtin_va_start(arg, format);

  buf[0] = (char) * __builtin_va_arg(arg, char*);
  buf[0] = (char) * __builtin_va_arg(arg, int*);
  // buf[0] = (char) * __builtin_va_arg(arg, wchar_t*);
  buf[0] = (char) * __builtin_va_arg(arg, float*);

  __builtin_va_end(arg);
}

int main()
{
  char *s;
  s = getenv("HOME");                               // BAD
  char buf[100];
  //   buf[0] = (char $tainted) 3;
  sprintf(buf, "%s", s);
  printf(buf);
}
