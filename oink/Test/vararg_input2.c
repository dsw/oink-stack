int printf(const char $untainted * fmt, ...) {}
char $tainted getchar() {}

int sprintf (char $_1 * buf, const char $untainted * format, ... $_1 );

int main()
{
  int c;
  c = getchar();                                    // BAD
  char buf[100];
  sprintf(buf, "%s", c);
  printf(buf);
}
