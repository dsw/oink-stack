int sprintf(char * $_1_2 * str, const char $untainted * format, ... $_1);

int main(void)
{
  char $tainted *x;
  char * * y;
  int a;

  sprintf(y, "%s %d", x, a);
}
