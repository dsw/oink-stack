$tainted char *getenv(const char *name);
int printf($untainted const char *fmt, ...);
int main(void)
{
  char *s, *t;
  s = getenv("LD_LIBRARY_PATH");
  //  t = s;
  printf(t);
}
