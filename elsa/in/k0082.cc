// const register function parameter

// originally found in package 'tripwire_2.3.1.2.0-4'

// In state 896, I expected one of these tokens:
//   <name>, bool, char, class, const, double, enum, float, int, long, operator, short, signed, struct, typename, union, unsigned, void, volatile, wchar_t, ::, __attribute__, __typeof__, restrict, _Complex, _Imaginary,
// a.ii:3:16: Parse error (state 896) at register

// ERR-MATCH: Parse error .state 896. at register

void foo(const register int c)
{
}

int main()
{
  foo(42);
}
