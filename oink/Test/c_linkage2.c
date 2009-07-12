// check that C linkage works, say if two functions have same names
// but different types; this is file 2

// NOTE: the whole point is that in the other file, this is "void f"
int f(int x) {
  int $untainted y;
  y = x;
}
