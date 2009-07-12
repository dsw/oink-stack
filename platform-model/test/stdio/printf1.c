
//@@ TESTS: TEST_PRINTF TEST_VPRINTF
//@@ TESTS: TAINT NO_TAINT

#include <stdio.h>
#include <stdarg.h>

int xprintf(char * fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

#if defined TEST_PRINTF
    printf(fmt);
#elif defined TEST_VPRINTF
    vprintf(fmt, arg);
#endif

    va_end(arg);
}

int main()
{
    char oops[100];

#if defined(TAINT)
    gets(oops); "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif

    xprintf(oops);
}
