// extern inline functions allow double-definition; the second is used
// to the exclusion of the first even before the second is seen and
// even though the first is supposed to be inline:
// http://gcc.gnu.org/onlinedocs/gcc-3.4.1/gcc/Inline.html#Inline

extern inline void f(int x) {}

int main() {
  int $tainted a;
  int $untainted b;
  b = f(a);
}

// NOTE: second definition for f
void f(int z) {
  return z;                     // bad
}
