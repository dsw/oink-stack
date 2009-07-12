//@@ TESTS: TEST_FREAD

#include <stdio.h>

struct S1 { char buf[20]; };

int main()
{
    struct S1 s1;
#if defined TEST_FREAD
    fread(&s1, sizeof(struct S1), 1, stdin);
    "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif
    printf(s1.buf);
}
