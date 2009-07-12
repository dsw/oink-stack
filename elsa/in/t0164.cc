// t0164.cc
// derived from t0078.cc, but with a ctor for hash

//    struct hash2 {
//      int x;
//      //hash2(hash2 &other) /*: x(other.x)*/ {}
//      hash2(hash2 &other);
//    };

// see also oink/Test/nested_no_def_constr1.cc
// (5b1d3a6a-1520-4765-95d4-3f53516f4e13)

struct Foo {
  struct hash {
    int x;
    hash(hash &other) : x(other.x) {}
    //hash(hash &other);
  } hash;
//    Foo(Foo &other) : hash(other.hash) {}
};
