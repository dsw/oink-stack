// This example from cqual/examples.
//  $tainted char *getenv(const char *name);
//  int printf($untainted const char *fmt, ...);

int main(void)
{
  char *s;
  char gronk;
  s = &gronk;
  s = &gronk;
}
