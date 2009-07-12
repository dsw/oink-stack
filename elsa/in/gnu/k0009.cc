// "namespace attribute strong"

// from /usr/include/c++/3.4/i486-linux-gnu/bits/c++config.h

// In state 706, I expected one of these tokens:
//   ;,
// k0009.cc:9:21: Parse error (state 706) at __attribute__

// ERR-MATCH: a86c8a1e-2cd7-41bd-8a23-f9049358c166

namespace g
{
}

namespace std
{
  using namespace g __attribute__ ((strong));
}
