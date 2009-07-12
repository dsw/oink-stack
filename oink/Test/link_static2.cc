
// foo and foo2 are undefined at link time, but called by bar and bar2
// respectively.

// bar and bar2 are static and unused.

// dsw: g++ also thinks that this is an unsatisfied symbol unless you
// run it with -O2

// old:
void foo();

// fixed by giving a definition; now g++ (with no -O2) likes it:
// void foo() {}

static void bar() {
  foo();
}

struct A
{
  void foo2();

  // bar2 has implicit static linkage since it's implicitly inline
  void bar2() { foo2(); }
//   void bar2();
};

// void A::bar2() { foo2(); }

int main() {}
