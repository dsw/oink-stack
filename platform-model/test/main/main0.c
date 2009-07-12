//@@ TESTS: TEST_MAIN0 TEST_MAIN1 TEST_MAIN2
//@@ TESTS: TEST_TAINT TEST_NULL

#include <stdio.h>

void danger(char *x)
{
    char *str;
#ifdef TEST_TAINT
    str = x;
    "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif
    printf(str);
}

#if 0
{ // indent
#endif

#if TEST_MAIN0
    // check that no-arg main links fine
    int main()
    {
        char buf[100];
        danger(gets(buf));
    }

#elif TEST_MAIN1
    int main(int argc, char **argv)
    {
        danger(argv[0]);
    }

#elif TEST_MAIN2

    int main(int argc, char **argv, char** environ)
    {
        danger(environ[0]);
    }

#else
    ERROR ERROR;
#endif
