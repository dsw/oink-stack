// template instantiation with all default args

// In state 664, I expected one of these tokens:
//   asm, try, (, ), [, ], ->, ::, ., +, -, ++, --, &, *, .*, ->*, /, %, <<, >>, <, <=, >, >=, ==, !=, ^, |, &&, ||, ?, :, =, *=, /=, %=, +=, -=, &=, ^=, |=, <<=, >>=, ,, ..., ;, {, }, __attribute__, <?, >?,
// a.ii:17:26: Parse error (state 664) at <name>: myint1

// from sigc++ deduce_result_type.h

template <class T=int>
struct S1 {
    typedef int myint;
};

struct S2 {
    template <class T=int>
    struct S2a {
        typedef int myint;
    };
};

template <class T>
struct S7 {
    typename S1<>::myint myint1;

    typename T::template S2a<>::myint myint2;
};

int main()
{
    S1<>::myint myint1;

    S2::S2a<>::myint myint2;

    S7<S2> s7;
}

