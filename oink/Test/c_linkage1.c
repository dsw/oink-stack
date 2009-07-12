// check that C linkage works, say if two functions have same names
// but different types; this is file 1

// NOTE: the whole point is that in the other file, this is "int f"
void f(int x);

int main() {
  int $tainted x;
  f(x);
}
