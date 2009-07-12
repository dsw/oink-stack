
//@@ TESTS: TEST_GETCHAR TEST_GETC TEST_FGETC TEST_GETC_MACRO_STDIN TEST_GETC_MACRO_FOPEN TEST_NULL TEST_NULL

#include <stdio.h>

int main()
{
    char c;
#if defined TEST_GETCHAR
    c = getchar();
    "EXPECT_TAINT_WARNINGS";
#elif defined TEST_GETC
    c = getc(stdin);
    "EXPECT_TAINT_WARNINGS";
#elif defined TEST_FGETC
    c = fgetc(stdin);
    "EXPECT_TAINT_WARNINGS";
#elif defined TEST_GETC_MACRO_STDIN
    c = _IO_getc_unlocked(stdin);
    "EXPECT_TAINT_WARNINGS";
#elif defined TEST_GETC_MACRO_FOPEN
    FILE* fp = fopen("/etc/passwd","r");
    c = _IO_getc_unlocked(fp);
    "EXPECT_TAINT_WARNINGS";
#elif defined TEST_NULL
    /* nothing */
    "EXPECT_NO_WARNINGS";
#else
    ERROR ERROR ERROR;
#endif

    printf(&c);
}
