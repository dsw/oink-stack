//@@ TESTS: TEST_SGETN TEST_NULL

#include <stdio.h>

int main()
{
    char buf[1024];

#if TEST_SGETN
    _IO_sgetn(NULL, buf, 100);
    "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif

    printf(buf);
}
