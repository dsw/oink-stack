// t0628.cc
// more complicated version of t0627.cc

// Reported by Umesh Shankar.

class Unrelated
{};

template <typename T>
class A {
public:
  typedef A<T> AT;

  AT & operator<< (int (*__pf) (AT &));

  int operator<< (Unrelated &);
};

void operator<< (A<char> &, char);


template <typename T>
int endl (A<T> &__os);


void Baz(A<char> &os)
{
  // What happens here is sort of complicated:
  //
  // 1. We tcheck "os << endl", and then rewrite it as
  // "os.operator<<(endl)" and re-tcheck it.
  //
  // 2. During the tcheck of "os.operator<<(endl)", we recognize the
  // 'endl' as having an implicit address-of, and therefore transform
  // it to "os.operator<<(&endl)".  Call this LHS; the version with
  // '&' is not re-tchecked yet.
  //
  // 3. We tcheck "LHS << 'a'", and then rewrite it as
  // "operator<<(LHS, 'a')", then re-tcheck it.
  //
  // 4. During the tcheck of LHS, which is "os.operator<<(&endl)",
  // we are doing what is shown in in/t0627.cc, and the '&' ends up
  // playing a role.
  //
  // Now, all of this of course is strong evidence that the strategy
  // of re-tchecking things is not a good idea, mainly because when
  // things fail it can be very non-obvious why (not to mention the
  // quadratic performance cost).  But, for the moment, I'm going to
  // leave it as-is.
  (os << endl) << 'a';
}

// EOF
