char $_1_2 *strcat1(char $_1_2 *s1, const char $_2 *s2);
char *strcat2(char $_1_2 *s1, const char $_2 *s2);
char $_1_2 *strcat(char $_1_2 *s1, const char $_2 *s2, char *x);

int main(void)
{
  $tainted char *x, *z, *a, *b, *e;
  char *y, *w, *c, *d, *f;

  char *a1 = strcat1(x, y);
  char *a2 = strcat2(w, z);
  strcat(a, c, e);
  strcat(d, b, f);
}
