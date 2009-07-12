
struct S {
  char * p;
};

char $tainted* getenv() {}
int printf(char $untainted *, ...) {}

int main()
{
  struct S * s1;
  struct S * s2;
  void * t;

  s1->p = getenv();                                 // BAD
  t = s1;
  s2 = t;

  printf(s2->p);
}
