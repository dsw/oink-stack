// t0624.cc
// "Assertion failed: got to implicit receiver code"

// Reported by Umesh Shankar.

class B {
public:
  template <class T>
  void baz(T *value);

  template <class T>
  void zoo(T *value);
};

template <>
void B::zoo(int *p)
{
}

template <>
void B::baz(int *p)
{
  zoo(p);
}

// EOF
