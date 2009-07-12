
// for easier debugging, use 'float' instead of 'int', to avoid int-as-void*
struct S {
  float a;
};

// use 'double' to avoid bug where 'void' return types have buckets
double set_a(struct S *s_s, float a)
{
  s_s->a = a;
}

float get_a(struct S *s_g)
{
  return s_g->a;
}
