typedef int (*func_t) ();
int bar(func_t f) {
  return f();
}
