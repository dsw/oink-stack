// ERR-MATCH: 1fe50763-449a-48ad-8664-04d04cc24322

// qual: Assertion failed: a->getEllipsis() == b->getEllipsis() &&
// "1fe50763-449a-48ad-8664-04d04cc24322", file dataflow_ty.cc line 220

typedef int (*EllipsisFunc) (int, ...);

int foo();

void main() {
  (EllipsisFunc) &foo;
  (EllipsisFunc) &foo;
}
