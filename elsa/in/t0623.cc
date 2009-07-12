// t0623.cc
// template class befriends template class

// Reported by Alexander Vodomerov.

template <typename type>
class A { };

template <typename type>
class B {
public:
  template <typename X>
  friend class A;
};

// EOF
