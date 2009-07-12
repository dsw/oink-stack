//@@ TESTS: TEST_TOUPPER TEST_TOUPPER_OPT TEST_NULL
//@@ TESTS: TEST_TAINT TEST_NOTAINT

#include <stdio.h>
#include <ctype.h>

// inline-optimized version of toupper (from gcc-3.4), which does an array
// index.  Use qualx --variant C to find this.
#  define OPT_tolower(c) ((int) (*__ctype_tolower_loc ())[(int) (c)])

int main()
{
    char c0, c1;
#if TEST_TAINT
    c0 = getchar();
    "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif

#if TEST_TOUPPER
    c1 = toupper(c0);
#elif TEST_TOUPPER_OPT
    c1 = OPT_tolower(c0);
#elif TEST_NULL
    c1 = c0;
#else
    ERROR ERROR;
#endif

    printf(&c1);
}
