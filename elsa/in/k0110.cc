// "calling template dtor with template arguments"

// from gcc-3.4 ios_init.cc

// a.ii:9:8: error: call site name lookup failed to yield any candidates; last
// candidate was removed because: non-template given template arguments

//ERR-MATCH: last candidate was removed because: non-template given template arguments

template<typename T>
struct S {
};

int main()
{
    S<char> s1;
    s1.~S<char>();
}
