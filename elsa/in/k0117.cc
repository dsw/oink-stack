// class templatized on int, copy constructor

// pstoedit_3.33-15/pstoedit.cpp.438c7efbc0cced5ef0b70ed3c5068ee8.ii:32656:40:
// error: more than one ambiguous alternative succeeds (from template; would
// be suppressed in permissive mode)

// ERR-MATCH: more than one ambiguous alternative succeeds

template <int e>
struct S {
  // S<e>(S<e> const &) {}

  void foo() {
    new S<e>(*this);
  }
};
