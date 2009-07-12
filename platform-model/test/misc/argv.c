//@@ TESTS: TAINT NO_TAINT

// Test that argv is tainted.

#include <stdio.h>

int main(int argc, char **argv)
{
    char *t;
#if defined TAINT
    t = argv[0];
    "EXPECT_TAINT_WARNINGS";
#else
    t = "";
    "EXPECT_NO_WARNINGS";
#endif
    printf(t);
}
