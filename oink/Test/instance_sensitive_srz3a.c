
struct S {
  int a;
};

extern void set_a(struct S *s, int a);

extern int get_a(struct S *s);

int main()
{
  struct S s1;
  struct S s2;

  int $tainted t;
  int $untainted u;

  set_a(&s1, t);

  // correct behavior:
  //     instance sensitivity on:  no warning
  //     instance sensitivity off: warning
  u = get_a(&s2);
}
