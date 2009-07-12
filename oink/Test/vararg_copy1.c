int xprintf(const char $untainted * fmt, ...) {}
char $tainted * xgetenv(const char* x) {}

int xsprintf (char* buf, const char $untainted * format, ...)
{
  __builtin_va_list arg;
  __builtin_va_list arg2;
  __builtin_va_start(arg, format);
  __builtin_va_copy(arg2, arg);

  buf[0] = * __builtin_va_arg(arg2, char*);

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
