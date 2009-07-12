// t0612.cc
// variant of t0611.cc that *is* valid

// primary
template <class T>
struct B
{};
 
// since this is just a pointer, we should not commit to a
// specialization yet
B<int*> *b;

// explicit (partial) specialization
template <class T>
struct B<T*>
{};

void f(B<int*> *c);

void g()
{
  B<int*> *c;

  // both should be pointers to the same type
  b = c;
  c = b;
  
  f(c);
  f(b);
}


// EOF
