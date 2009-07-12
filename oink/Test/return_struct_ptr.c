char $tainted* getenv(char*) {}
int printf(char $untainted*, ...) {}

struct S {
  char* buf;
};

struct S $tainted * getS() {
  struct S * s;
  char $tainted c;
  s->buf[0] = c;                                    // BAD
  return s;
}

int main()
{
  struct S* t = getS();
  printf(t->buf);

}
