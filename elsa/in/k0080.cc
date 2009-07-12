// already implicitly instantiated

// originally found in package 'aspell_0.60.2+20050121-2'

// b.ii:13:27: error: S2<void /*anon*/> has already been implicitly instantiated, so it's too late to provide an explicit specialization

// ERR-MATCH: has already been implicitly instantiated

struct S1 {};

template <typename T1> struct S2 : S1 {
  int foo(S1 const &);

  int bar(S2<void> const & other) {
    foo(other);
  }
};

template <> struct S2<void> : S1 {};
