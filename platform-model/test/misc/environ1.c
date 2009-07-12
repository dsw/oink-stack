//@@ TESTS: TAINT NO_TAINT

// Test that environ is tainted.

#include <stdio.h>

int main(int argc, char **argv, char **env)
{
    char *t;
#if defined TAINT
    t = env[0];
    "EXPECT_TAINT_WARNINGS";
#else
    t = "";
    "EXPECT_NO_WARNINGS";
#endif
    printf(t);
}
