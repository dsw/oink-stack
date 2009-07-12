//@@ TESTS: TEST_SCANF TEST_NULL

#include <stdio.h>

int main()
{
    char buf[100];
#ifdef TEST_SCANF
    scanf("%s", buf);
    "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif
    printf(buf);
}
