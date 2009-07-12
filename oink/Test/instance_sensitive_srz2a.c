
struct S {
  float a;
};

extern double set_a(struct S *s_s_p, float a);

extern float get_a(struct S *s_g_p);

double main()
{
  struct S s1_main;

  float $tainted t;
  float $untainted u;

  set_a(&s1_main, t);

  // correct behavior:
  //     instance sensitivity on:  warning (FAILS)
  //     instance sensitivity off: warning
  u = get_a(&s1_main);
}
