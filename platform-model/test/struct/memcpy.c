//@@ TESTS: TEST_MEMCPY TEST_NULL
//@@ TESTS: TEST_TAINT TEST_NOTAINT

// #if __cplusplus
// # define INSTANCE_SENSITIVITY 0
// #else
// # define INSTANCE_SENSITIVITY 1
// #endif

// quarl 2006-08-16
//    Since instance sensitivity is always-off right now, this test case will
//    always pass without testing whether memcpy works.  It does (should) work
//    even with instance sensitivity on, though.
#define INSTANCE_SENSITIVITY 0

#include <stdio.h>
#include <string.h>

struct S1 { char buf[20]; };

int main()
{
    struct S1 s1a;
    struct S1 s1b;

#if TEST_TAINT
    gets(s1a.buf);
#endif

#if TEST_MEMCPY
    memcpy(&s1b, &s1a, sizeof(struct S1));
#endif

#if TEST_TAINT && (TEST_MEMCPY || !INSTANCE_SENSITIVITY)
    // if we memcpy, then s1b should be tainted.
    // if we're in C++, then instance sensitivity is off, so should be tainted.
    "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif

    printf(s1b.buf);
}
