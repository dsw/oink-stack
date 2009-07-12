// reference to reference used in inner class of template class

// I think this is correctly typechecked as error; allow with permissive mode
// and/or indicate this is allowed by gcc bug?

// originally found in package 'wvstreams_4.0.2-4'

// b.ii:9:18: error: cannot create a reference to a reference (inst from b.ii:13:10)

// ERR-MATCH: cannot create a reference to a reference

template<typename T1>
struct S1 {
  struct S2 {
    void foo (T1 &t1) {}
  };
};

S1<int&> s1;

