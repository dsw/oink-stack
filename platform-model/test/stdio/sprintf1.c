
//@@ TESTS: TEST_NULL TEST_SPRINTF_FS TEST_SPRINTF_STR
//@@ TESTS: TAINT NO_TAINT

//@XFAIL: TEST_SPRINTF_STR,TAINT

#include <stdio.h>
#include <stdarg.h>

int xsprintf(char * buf, char * fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
#if defined TEST_VSPRINTF_FS
    vsprintf(buf, fmt, arg);
#elif defined TEST_VSPRINTF_STR
    vsprintf(buf, fmt, arg);
    printf(buf);
#endif
    va_end(arg);
}

int main()
{
    char oops[100];
    char buf[100];

#if defined(TAINT)
    gets(oops); "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif

#if defined TEST_NULL
    printf(oops);
#elif defined TEST_SPRINTF_FS
    sprintf(buf, oops);
#elif defined TEST_SPRINTF_STR
    sprintf(buf, "%s", oops);
    printf(buf);
#elif defined TEST_VSPRINTF_FS || TEST_VSPRINTF_STR
    xsprintf(buf, oops);
#endif
}
