// ERR-MATCH: 518c9b2c-5c46-423b-b2e4-1f9620fe1cf7

// qual: Assertion failed: src0->params->count() == tgt0->params->count() && "518c9b2c-5c46-423b-b2e4-1f9620fe1cf7", file qual_dataflow_ty.cc line 91

typedef int (*foo_func_t)(int foo1, int foo2, int foo3);
typedef int (*bar_func_t)(int bar1, int bar2);

typedef int (*func_t)();

int main() {
  foo_func_t foo;
  bar_func_t bar;

  func_t f;

  f = foo;
  f = bar;
}

