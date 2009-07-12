// scoping via template instantiation with all parameters defaulted

// originally found in package 'libcrypto++'

// In state 646, I expected one of these tokens:
//   asm, try, (, ), [, ], ->, ::, ., +, -, ++, --, &, *, .*, ->*, /, %, <<, >>, <, <=, >, >=, ==, !=, ^, |, &&, ||, ?, :, =, *=, /=, %=, +=, -=, &=, ^=, |=, <<=, >>=, ,, ..., ;, {, }, __attribute__, <?, >?,
// a.ii:11:11: Parse error (state 646) at <name>: s

// ERR-MATCH: In state (646|658), I expected one of these tokens

template <typename T = int>
struct S
{
  struct S2 {};
};

int foo()
{
  S<>::S2 s;
}
