// instantiation of template member function via implicit address-of-function

// see also elsa/in/k0105.cc
// (0a257264-c6ec-4983-95d0-fcd6aa48a6ce|ee42ebc5-7154-4ace-be35-c2090a2821c5)

template <typename T>
struct S1 {
  static void foo();
};

template <typename T>
void S1<T>::foo()
{
}

typedef void (*func)(void);

int main()
{
  func f = /* missing "&" */ S1<int>::foo;
}
