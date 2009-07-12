
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
