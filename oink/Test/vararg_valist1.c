int xprintf(const char $untainted * fmt, ...) {}
char $tainted * xgetenv(const char* x) {}

int xvsprintf(char* buf, __builtin_va_list arg)
{
  buf[0] = * __builtin_va_arg(arg, char*);
}

int xsprintf (char* buf, const char $untainted * format, ...)
{
  __builtin_va_list arg;
  __builtin_va_start(arg, format);

  xvsprintf(buf, arg);

  __builtin_va_end(arg);
}

int main()
{
  char *s = "";
  char buf[100];
  s = xgetenv("HOME");                               // BAD
  xsprintf(buf, "%s", s);
  xprintf(buf);
}
