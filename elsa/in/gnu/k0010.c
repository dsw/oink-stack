// nested K&R function

// Assertion failed: unimplemented: nested K&R function definition
// (d00aa531-ca12-4d72-9caf-ca9db9941179), file gnu.gr line 144

// ERR-MATCH: d00aa531-ca12-4d72-9caf-ca9db9941179

void foo (void) {
  void bar (x) int x;
  {
  }
}
