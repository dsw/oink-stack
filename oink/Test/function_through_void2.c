// #define $tainted
// #define $untainted

typedef int (MyFuncB) (int x);

int f (int x) {
  return x;
}
     
int main() {
  void *p;
  p = &f;
  MyFuncB *f2;
  f2 = p;                       // same as function_through_void1.c but without cast

  int $tainted t;
  int $untainted u;

  u = f2(t);
}
