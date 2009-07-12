// Test that attribute((alias)) works.
//
// File should link and report taint.

// #define $tainted
// #define $untainted

int bar(int x);
extern int bar(int x) __attribute__((alias("foo")));

// foo has to be defined in this translation unit, but doesn't have to come
// before bar.
int foo(int x) {
  return x;
}

// example real code from glib2.0/gmem.c:
//     extern __typeof (g_free) g_free __attribute((alias("IA__g_free"),
//     visibility("default")));

int main()
{
  int $tainted t;
  int $untainted u;

  u = bar(t);
}
