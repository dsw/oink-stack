int printf(const char $untainted * fmt, ...) {}
char $tainted * getenv(const char* x) {}

int sscanf (const char* s, const char $untainted * format, ...)
{
  __builtin_va_list arg;
  __builtin_va_start(arg, format);

  * __builtin_va_arg(arg, char*) = (char) s[0];
  * __builtin_va_arg(arg, int*) = (int) s[0];
  // * __builtin_va_arg(arg, wchar_t*) = (wchar_t) s[0];
  * __builtin_va_arg(arg, float*) = (float) s[0];

  __builtin_va_end(arg);
}

int main()
{
  char *s;
  s = getenv("HOME");                               // BAD
  char buf[100];
  sscanf(s, "%s", buf);
  printf(buf);
}
