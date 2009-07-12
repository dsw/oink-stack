// ERR-MATCH: a09c81ae-0867-450a-9489-4a31932c25d9

// pure virtual functions can have definitions, apparently.

struct S {
  virtual int foo(int x) = 0;
};

int S::foo(int x) {
  return x;
}

struct S2 : S {
  virtual int foo() { return 42; }
};

int main()
{
  S *s = new S2;

  int $tainted t;
  int $untainted u;

  // Note: S::foo() could also be called without explicitly qualifying with
  // S:: if called from S's constructor (or a function the constructor calls)
  u = s->S::foo(t);
}
