// ERR-MATCH: 25f0c1af-5aea-4528-b994-ccaac0b3a8f1

// Assertion failed: IC_AMBIGUOUS -- what does this mean here?
// (25f0c1af-5aea-4528-b994-ccaac0b3a8f1), file cc_tcheck.cc line 5894

void Foo(const void *p);

struct S {
  operator int *() {};
  operator void *() {};
};

int main() {
  S s;
  Foo(s);
}
