// ERR-MATCH: Target function has ellipsis but source function does not.

// ** no prototype for sprintf

#ifndef __GNUC__
int printf(const char $untainted * fmt, ...) {}
char $tainted * getenv(const char* x) {}
#endif

int sprintf();

int main()
{
  char * s = getenv("HOME");
  char t[100];

  sprintf(t, "%s", s);                         // BAD

  printf(t);
}
