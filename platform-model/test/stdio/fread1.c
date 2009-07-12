
//@@ TESTS: TEST_FREAD TEST_NULL

#include <stdio.h>

int main()
{
    char c;
#if TEST_FREAD
    fread(&c, 1, 1, stdin);
    "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif

    printf(&c);
}
