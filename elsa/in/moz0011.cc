struct __true_type { };
struct __false_type { };

template<typename, typename>
  struct __are_same
  {
    enum { __value = 0 };
    typedef __false_type __type;
  };

template<typename _Tp>
  struct __are_same<_Tp, _Tp>
  {
    enum { __value = 1 };
    typedef __true_type __type;
  };

class A {
};

int p = __are_same<A, A>::__value;
