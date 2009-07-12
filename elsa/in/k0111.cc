// "duplicate template specialization/definition of static data"

// a.cc:8:24: error: duplicate definition for `foo' of type `char const *';
// previous at a.cc:6:23 (from template; would be suppressed in permissive
// mode)

// from gcc-3.4 locale_facets.cc

//ERR-MATCH: duplicate definition.*from template

template <class T>
struct S {
    static const char *foo;
};

template<> const char *S<char>::foo;

template<> const char *S<char>::foo = "foo";

