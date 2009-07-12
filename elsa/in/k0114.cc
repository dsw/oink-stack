// template parameterized on function (not function pointer)

// ERR-MATCH: 6ccc991e-bd8a-47d8-8f5c-e75d7065a29d

// Assertion failed: origSrc && "6ccc991e-bd8a-47d8-8f5c-e75d7065a29d", file
// template.cc line 3778 a.ii.16

// first seen in gwenview/gvimageutils.cpp.50e0d6a9383e1f3a615b9b5350d2597b.ii

int foo( int x1, int x2 ) {
}

typedef int (Function_t)( int, int );

template< Function_t function >
int bar() {
}

int main()
{
  bar< foo > ();
}
