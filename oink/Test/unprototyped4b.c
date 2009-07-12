int sprintf(char * buf, char const* fmt, ...)
{
  __builtin_va_list arg;
  __builtin_va_start(arg, fmt);
  buf[0] = * __builtin_va_arg(arg, char*);
}
