// ERR-MATCH: cannot convert argument type `.*const

typedef float FloatArray[];

// (gcc thinks?) const is pushed down the type tree since it doesn't make
// sense to have the const apply to the array
int foo (const FloatArray f) {}

int main()
{
  float const *f;
  foo(f);
}
