
//@@ TESTS: TEST_0 TEST_PUTCHAR TEST_PUTC TEST_FPUTC
//@@ TESTS: TAINT NO_TAINT

#include <stdio.h>

int main()
{
    char c0;
#if defined(TAINT)
    c0 = getchar(); "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif

    char c;
#if defined TEST_0
    c = c0;
#elif defined TEST_PUTCHAR
    c = putchar(c0);
#elif defined TEST_PUTC
    c = putc(c0, stdout);
#elif defined TEST_FPUTC
    c = fputc(c0, stdout);
#else
    ERROR ERROR ERROR;
#endif

    printf(&c);
}
