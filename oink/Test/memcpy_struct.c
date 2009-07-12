struct S {
  char * p;
};

int printf(char $untainted * fmt, ...) {}

const char $tainted* gets() {}

void* memcpy(void $_1_2 * dest, void $_1 * src, int length) {}

int main()
{
  struct S s1;
  s1.p = gets();                                    // BAD

  struct S s2;
  // s2.p = s1.p;
  memcpy(&s2, &s1, sizeof(struct S));
  printf(s2.p);
}
