// #define $tainted
// #define $untainted

typedef int (MyFuncB) (int x);

int f (int x) {
  return x;
}
     
int main() {
  MyFuncB *f2;
  f2 = (MyFuncB*) &f;

  int $tainted t;
  int $untainted u;

  u = f2(t);
}
