// typename ParameterDeclaration in TemplateParameterList

// ERR-MATCH: 5fb56c83-4701-421f-9f6a-f9333f0aef56

// from boost, originally seen in package 'lyx'

// Assertion failed: unimplemented: typename ParameterDeclaration in
// TemplateParameterList (5fb56c83-4701-421f-9f6a-f9333f0aef56), file cc.gr
// line 2383

namespace boost {
  struct A
  {
    typedef int fast;
  };
}

struct A2
{
  typedef int fast;
};

template < typename ::boost::A::fast foo = 0u >
struct S {};

template < typename A2::fast foo >
struct S2 {};
