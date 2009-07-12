// ERR-MATCH: 19b23af4-9169-459b-a3c2-90c68ad0acc7

// Assertion failed: var0 && "19b23af4-9169-459b-a3c2-90c68ad0acc7", file
// qual_var.cc line 87

int foo ();

typedef int (*Func)();

struct DUMMY {};

template <Func f>
struct S {
};

typedef S<&foo> S1foo;
