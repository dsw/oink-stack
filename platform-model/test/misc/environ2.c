//@@ TESTS: TAINT NO_TAINT

// Test that environ is tainted.

#include <stdio.h>

extern char **environ;

int main()
{
    char *t;
#if defined TAINT
    t = environ[0];
    "EXPECT_TAINT_WARNINGS";
#else
    t = "";
    "EXPECT_NO_WARNINGS";
#endif
    printf(t);
}
