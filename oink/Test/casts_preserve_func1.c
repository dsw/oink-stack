int main() {
  typedef int gronk(int $untainted);
  typedef int foo(int $tainted);
  typedef int foo_noqual(int);
  gronk *g;
  foo *f;
  // NORMAL: flow
//   f = g;
  // QUALIFIER IN CAST (hidden in the typedef): no flow
//   f = (foo*)g;
  // NORMAL CAST: flow unless -fq-no-casts-preserve-below-functions
  f = (foo_noqual*)g;
}

// int main() {
//   int $tainted x;
//   int $untainted y;
//   // NORMAL: flow
//   y = x;
//   // QUALIFIER IN CAST: no flow
//   y = (int $untainted) x;
//   // NORMAL CAST: flow
//   y = (int) x;
// }
