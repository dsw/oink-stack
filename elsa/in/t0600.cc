// t0600.cc
// based on a bug report by Alexander Vodomerov

// was running into a problem where a PseudoInstantiation (the
// self-name, I think) was being created whose 'primary' pointed to a
// specialization rather than a primary

template <typename T1, typename T2> 
struct pair { };

template <typename T> 
struct test {
  typedef T my_type;
};

template <typename T1, typename T2>
struct test<pair<T1, T2> > {
  typedef typename test<T1>::my_type my_type;
  my_type x;
};

void f()
{
  // make sure they can be instantiated
  test<int> t1;
  test<pair<int, float> > t2;
  
  // and that we get the right types for 'my_type'
  int y;
  __elsa_checkType(t2.x, y);
}

// EOF
