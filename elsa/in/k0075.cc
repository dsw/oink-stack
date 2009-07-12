// overloaded function involving function pointer in presence of template

// originally found in package 'digikam_0.7.2-2'

// Assertion failed: argType != NULL && "52291632-1792-4e5c-b5b8-9e6240b75a91", file template.cc line 1305
// Failure probably related to code near a.ii:18:3
// current location stack:
//   a.ii:18:3
//   a.ii:15:1
//   a.ii:14:5

// ERR-MATCH: Assertion failed:.*52291632-1792-4e5c-b5b8-9e6240b75a91

struct S1 {};
struct S2 {};

int operator<<(S1, S1 (*f)(S1)) {}

inline S1 foo(S1 s) {}
inline S2 foo(S2 s) {}

template <typename T1> struct SB;
template <class T1> inline char operator<<(char, const SB<T1>& p) {}

int main()
{
  S1 s;
  int i;
  s << foo;
}
