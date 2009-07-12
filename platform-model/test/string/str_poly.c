//@@ TESTS: TEST_0 TEST_1

// tests polymorphism

#include <string.h>
#include <stdio.h>

int main()
{
    char buf1[100];
    char buf2[100];
    gets(buf1);
    strcpy(buf2, buf1);

    char buf3[100];
    char buf4[100];

#if defined TEST_0
    strcpy(buf4, buf1);
    "EXPECT_TAINT_WARNINGS";
#else
    strcpy(buf4, buf3);
    "EXPECT_NO_WARNINGS";
#endif

    printf(buf4);
}
