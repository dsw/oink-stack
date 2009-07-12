// t0581.cc
// a simpler (than t0435.cc) template array bounds example

template <int ArraySize>
int *end(int(&array)[(ArraySize)])    // deliberately using grouping parens
{
  // confirm we deduced '2' as the size
  int blah[ArraySize==2? 1 : -1];
  return ArraySize;
}

void f()
{
  int dummy[2];
  end(dummy);
}


// slightly funky variation: the instantiation is triggered by its own
// definition
template <int ArraySize>
int *zend(int(&array)[ArraySize])
{
  int dummy[2];
  zend(dummy);
}


// EOF
