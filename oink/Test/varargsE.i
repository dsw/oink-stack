typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;
static char error_buf[1024];
int vsprintf_R954cbb26(char *buf, const char *, va_list);
void ufs_warning(const char * function, const char * fmt, ...)
{
  va_list args;
  __builtin_stdarg_start((args),fmt);
  vsprintf_R954cbb26 (error_buf, fmt, args);
  __builtin_va_end (args);
}
