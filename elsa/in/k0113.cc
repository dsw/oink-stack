// calling templatized operator-overloaded function

// In state 161, I expected one of these tokens:
//   <name>,
// b.ii:15:22: Parse error (state 161) at operator

// from sigc++-2.0/sigc++/functors/slot.h

//ERR-MATCH: fc042c37-50e2-4596-8460-c095e7ac892b

template <class T>
struct S1 {
    template <class T2>
    void operator()() {}

    template <class T2>
    void foo() {}
};

template <class T>
struct S2 {
    void foo() {
        S1<T> *s1;
        s1->template operator()<int> ();
    }
};

int main()
{
    S1<int> s1;
    s1.foo<int>();
    s1.operator()<int>();

    S2<int> s2;
    s2.foo();
}
