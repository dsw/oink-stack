// "instantiating template member function"

// a.ii:6:15: error: type `void ()(int )' does not match any template function
// `S::foo'

// seen in gcc-3.4/libstdc++

struct S {
    template<typename T> void foo(T) {}
};

template void S::foo(int);

