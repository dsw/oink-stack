// t0607.cc
// supply wrong # of args to function template

template <class T, class U>
int f();

void g()
{
  //ERROR(1): f<int>();
  f<int,int>();
  //ERROR(3): f<int,int,int>();
}


// EOF
