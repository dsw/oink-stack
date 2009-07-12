// THIS CURRENTLY FAILS IF USING SERIALIZATION (i.e. stdio.qz instead of
// stdio.c)

//@@ TESTS: TEST_1 TEST_2
//@@ TESTS: TAINT NO_TAINT

#include <stdio.h>

void printf_from_file_read(FILE* fp)
{
    char c;
#if defined(TAINT)
    c = fp->_IO_read_ptr [0];
    "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif

    printf(&c);
}

int main()
{
#if defined(TEST_1)
    printf_from_file_read(stdin);
#elif defined(TEST_2)
    printf_from_file_read(fopen("/etc/passwd","r"));
#else
#   error "No test selected"
    ERROR ERROR ERROR;
#endif
}
