// class friend main

// originally found in package 'bibletime'.

// Assertion failed: prior->hasFlag(DF_EXTERN_C), file cc_env.cc line 3505
// Failure probably related to code near k0063.cc:10:14
// current location stack:
//   k0063.cc:10:14
//   k0063.cc:9:1

// ERR-MATCH: c524f127-19cb-44eb-a829-f49faf2185a4
// ERR-MATCH: Assertion failed: prior->hasFlag.DF_EXTERN_C.

struct S {
  friend int main(int argc, char* argv[]);
};

