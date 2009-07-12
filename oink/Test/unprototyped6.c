// ERR-MATCH: e9d062c2-5446-4a9e-9ec3-dc41298d2b7d

// qual: Assertion failed: !f1Params.isDone() &&
// "e9d062c2-5446-4a9e-9ec3-dc41298d2b7d", file dataflow_visitor.cc line 89

// SomeFunc is first declared FF_NOPARAM, then matched up to something with
// params, then defined with less parameters.

typedef int (*myfunc)( int );

int SomeFunc();

myfunc f = SomeFunc;

int SomeFunc() {}
