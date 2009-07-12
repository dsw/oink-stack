// error: no template parameter list supplied for `A'

// note that the 'static' is critical for the bug

// sm: This is actually invalid C++, because there should be
//   template <>
// before the definition of A<int>::s.  However, gcc-2 accepts
// it (a bug), as does icc (apparently for compatibility with
// the gcc bug), though gcc-3 does not.  So, Elsa will probably
// have to support this too, if icc found it necessary.


template<class T> class A {
  static int s;
};

//template<>    <-- this is what is missing
int A<int>::s = 0;
