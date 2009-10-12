// moz0010.cc
// Use of a conversion operator yielding a pointer to an incomplete
// template type.
//
// This is fine, but Elsa used to complain about not being able to
// instantiate that type even though it didn't actually need to.

template<typename T>
class DPtr : public T {
};

template<typename T>
class MyPtr {
  T *mPtr;
public:
  operator DPtr<T>*() { return reinterpret_cast<DPtr<T>*>(mPtr); }
};

class Foo;

int main()
{
  MyPtr<Foo> p;
  if (1 || p) {
  }
}

