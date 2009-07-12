// ERR-MATCH: 8efc322b-1a2b-42c6-a387-ba23f9ce30ee

// qual: Assertion failed: tgtRetValue->t()->isCompoundType() &&
// "8efc322b-1a2b-42c6-a387-ba23f9ce30ee", file dataflow_ty.cc line 248

// Note: 8efc322b-1a2b-42c6-a387-ba23f9ce30ee is known to occur in two Debian
// packages (geomview, gnotime) and both of them are bugs in the
// code-under-analysis.

struct S {};

S foo2() {}

typedef int (*myfunc) ();

int main() {
  myfunc f = (myfunc) foo2;
}


