// t0585.cc
// problem with explicit dtor invocation on template class

// Accepted by both ICC and GCC.

template <class T>
struct A {
    A();
    ~A();
};

int main()
{
    A<int> a;

    // error: in ( (a).A<int /*anon*/>::~A), A was found in the
    // current scope as struct A, and also in the class of (a) as
    // struct A<int /*anon*/>, but they must be the same
    a.A<int>::~A();
}
