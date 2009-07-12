// __offsetof__

// http://gcc.gnu.org/ml/gcc-patches/2003-12/msg01317.html

//      @smallexample
//        __offsetof__ (expression)
//      @end smallexample
//
//      is equivalent to the parenthesized expression, except that the
//      expression is considered an integral constant expression even if it
//      contains certain operators that are not normally permitted in an
//      integral constant expression.  Users should never use
//      @code{__offsetof__} directly; the only valid use of
//      @code{__offsetof__} is to implement the @code{offsetof} macro in
//      @code{<stddef.h>}.

// originally found in package ddd

// k0006.cc:35:12: error: there is no function called `__offsetof__'
// typechecking results:
//   errors:   1
//   warnings: 0

// ERR-MATCH: error: there is no function called `__offsetof__'

struct S {
    int x;
    int y;
};

int main()
{
    struct S s;
    return __offsetof__((static_cast<S*> (0))->y);
}

int foo()
{
  // make sure it's a compile-time constant
  int static_assertion[ (4 == __offsetof__((static_cast<S*> (0))->y)) ? 1 : -1 ];
}
