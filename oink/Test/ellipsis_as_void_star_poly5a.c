// check that int-as-void* works across serialization

void f(char q, ...) {
  __builtin_va_list args;
  __builtin_va_start(args, q);
  float $untainted * fp = __builtin_va_arg(args, float*);
  __builtin_va_end;
}
