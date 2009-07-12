// ERR-MATCH: 60a04156-a119-4c6c-8c04-bca58e69dee1

// Assertion failed: primaryTI->isPrimary() &&
// "60a04156-a119-4c6c-8c04-bca58e69dee1", file template.cc line 2635

template<class T>
struct S {
};

template<>
struct S<int> {
  static int foo();
};

template<class T>
struct S<T*> {
  int bar() {
    S<int>::foo();
  }
};

int main()
{
  S<int*> s;
  s.bar();
}
