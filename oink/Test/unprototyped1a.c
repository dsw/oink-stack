// ** no prototype for strcpy

#ifndef __GNUC__
int printf(const char $untainted * fmt, ...) {}
char $tainted * getenv(const char* x) {}
#endif

int main()
{
  char * s = getenv("HOME");
  char t[100];

  strcpy(t, s);                                     // BAD

  printf(t);
}
