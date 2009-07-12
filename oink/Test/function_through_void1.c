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
  f2 = (MyFuncB*) p;

  int $tainted t;
  int $untainted u;

  u = f2(t);
}
