// t0596.cc
// demonstrate problem with operator overload resolution attempting to
// deduce template parameters from the name of an overloaded function

int foo(int);
int foo(float);

template <class T>
void bar(int (*f)(T));

struct S {} s;

template <class T>
void operator<<(S&, int (*f)(T));

void baz()
{
  // this works, I'm not sure why
  bar(foo);

  // this does not
  s << foo;
  
  // at a guess, it will also fail for E_constructors, since
  // they follow the same lookup path as overloaded operators
  
  // Interestingly, gcc-3.4.3 cannot handle either of the above.
}


// EOF
