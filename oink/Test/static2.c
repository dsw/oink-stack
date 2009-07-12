int gronk(int x);               // bad: extern delared but static defined
static int gronk(int x) {
  int x;
  int y;
  y = gronk(x);
}
static int foo(int x) {
  int x;
  int y;
  y = gronk(x);
}
int foo(int x);                 // bad: extern delared but static defined
