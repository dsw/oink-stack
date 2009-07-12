int xprintf(const char * fmt, ...) {}
char * xgetenv(const char* x) {}

int xvsprintf(char* buf, __builtin_va_list arg)
{
  __builtin_va_list arg2;
  __builtin_va_copy(arg2, arg);
  buf[0] = * __builtin_va_arg(arg2, char*);
}

int xsprintf (char* buf, const char * format, ...)
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
  s = xgetenv("HOME");                               
  xsprintf(buf, "%s", s);
  xprintf(buf);
}
