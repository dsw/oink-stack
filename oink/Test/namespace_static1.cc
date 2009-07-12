// ERR-MATCH: f5e7f910-2486-4fb8-b463-a91b5e6f8aa1

// qual: Assertion failed: (!first->hasFlag(DF_STATIC) ||
// first->hasFlag(DF_MEMBER)) && "f5e7f910-2486-4fb8-b463-a91b5e6f8aa1", file
// oink.cc line 581

namespace N {
  static int foo;
}
