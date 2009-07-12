
// Test single file instance sensitive serialization

struct S {
  int a;
};

void set_a(struct S *s, int a)
{
  s->a = a;
}

int get_a(struct S *s)
{
  return s->a;
}

int main()
{
  struct S s1;

  int $tainted t;
  int $untainted u;

  set_a(&s1, t);

  // correct behavior:
  //     instance sensitivity on:  warning (FAILS)
  //     instance sensitivity off: warning
  u = get_a(&s1);
}
