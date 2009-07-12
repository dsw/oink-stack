//@@ TESTS: TEST_CLONE TEST_NULL

// Tests that clone() calls its function argument.

#include <stdio.h>
#include <sched.h>

int foo(void *arg)
{
    printf((char*) arg);
}

int main()
{
    char buf[20];
    gets(buf);

#if defined TEST_CLONE
    "EXPECT_TAINT_WARNINGS";
    clone(&foo, 0, 0, buf);
#elif defined TEST_NULL
    "EXPECT_NO_WARNINGS";
#else
#   error
#endif
}
