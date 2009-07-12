// From Umesh Shankar <ushankar@google.com>

class Bar {
  Bar(bool (*func)(void *), void *arg);

  template<typename T>
  Bar(bool (*func)(T *), T *arg);

  template<typename T>
  Bar(T *object, bool (T::*method)());
};

template<typename T>
static bool foo(void *v, bool (Bar::*m)()) {
  typedef bool (T::*RealMethodType)();
  T *x = static_cast<T *>(v);
  RealMethodType rm = reinterpret_cast<RealMethodType>(m);
  return (x->*rm)();
}
