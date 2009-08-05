char $tainted *getenv(char const *name);
int printf(char const $untainted *fmt, ...);
int main(void)
{
  char *s, *t;
  s = getenv("LD_LIBRARY_PATH");
  t = s;
  printf(t);
}
