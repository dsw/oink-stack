// quarl 2006-07-13
//    auto union with different pointer types, function argument types, etc.

typedef char T1;
typedef int T2;

int main()
{
  int $tainted t;
  int $untainted u;

  T1 t1;
  void *x;
  T2 t2;

  t1 = t;

  *((T1*) x) = t1;
  t2 = *((T2*) x);

  u = t2;                       // BAD
}
