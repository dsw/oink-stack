$tainted char *getenv(const char *name);
int printf($untainted const char *fmt, ...);
int main(void)
{
  char *s;
  s = getenv("LD_LIBRARY_PATH");
  printf(s);
}
