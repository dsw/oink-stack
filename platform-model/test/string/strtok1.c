
//@@ TESTS: TEST_STRTOK TEST_NULL
//@@ TESTS: TAINT NO_TAINT

#include <string.h>
#include <stdio.h>

int main()
{
    char needle[100];


#if defined(TAINT)
    gets(needle); "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif

    char haystack[] = "foo bar blah";

#if defined(TEST_STRTOK)
    strtok(haystack, needle);
#else
    haystack[0] = needle[0];
#endif

    printf(haystack);
}
