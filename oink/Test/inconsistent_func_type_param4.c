// ERR-MATCH: 2679bf5a-87cb-40ca-891f-28e060d30cf3

// qual: a.i:11: Source function has ellipsis but target function does not
// (2679bf5a-87cb-40ca-891f-28e060d30cf3).

typedef int (*Func) (int, float);

int Foo ();                    // no param

void main() {
  Func foo;
  foo = &Foo;

  int $tainted t;
  int $untainted u;

  u = foo(t, 1.0);
}

int Foo (int x) {
  return x;
}
