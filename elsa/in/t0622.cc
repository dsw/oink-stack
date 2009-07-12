// t0622.cc
// problem with "friend class C" inside a template class

// Reported by Alexander Vodomerov.

template <typename type>
class A {
public:
  // The problem here is Elsa was basically ignoring the 'friend',
  // and declaring B as an inner (and hence templatized) class.
  friend class B;
};

class B {};

template <typename T>
void func()
{
}

void test()
{
  func<B>();
}

// EOF
