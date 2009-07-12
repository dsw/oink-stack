// t0619.cc
// like t0612.cc, but with a complete (not partial) specialization

// primary
template <class T>
struct B
{};
 
// since this is just a pointer, we should not commit to a
// specialization yet
B<int*> *b;

// explicit complete specialization
template <>
struct B<int*>
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
