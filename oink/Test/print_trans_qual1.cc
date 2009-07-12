// This was submitted by Matt Harren; it was causing
// -f-print-quals-graph to have an assertion failure.
struct list {
  int x;
  inline list() {}
};
