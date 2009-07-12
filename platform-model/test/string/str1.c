
//@@ TESTS: TEST_NULL TEST_STRCPY TEST_STRCPY2 TEST_STRCAT TEST_STRCHR TEST_STRDUP TEST_STRPBRK TEST_MEMCPY TEST_MEMCPY2 TEST_MEMMOVE TEST_MEMSET TEST_MEMCHR
//@@ TESTS: TAINT NO_TAINT

#include <string.h>
#include <stdio.h>

int main()
{
    char danger[100];
    char safe[100];
    char other[100];

#if defined(TAINT)
    gets(danger); "EXPECT_TAINT_WARNINGS";
#else
    "EXPECT_NO_WARNINGS";
#endif

#if defined TEST_NULL
    *safe = *danger;
#elif defined TEST_STRCPY
    strcpy(safe, danger);
#elif defined TEST_STRCPY2
    *safe = *strcpy(other, danger);
#elif defined TEST_STRCAT
    strcat(safe, danger);
#elif defined TEST_STRCHR
    *safe = *strchr(danger, 'x');
#elif defined TEST_STRDUP
    *safe = *strdup(danger);
#elif defined TEST_STRPBRK
    *safe = *strpbrk(danger, "x");

#elif defined TEST_MEMCPY
    memcpy(safe, danger, 100);
#elif defined TEST_MEMCPY2
    *safe = *( (char*) memcpy(other, danger, 100) );
#elif defined TEST_MEMMOVE
    memmove(safe, danger, 100);
#elif defined TEST_MEMSET
    memset(safe, danger[0], 100);
#elif defined TEST_MEMCHR
    *safe = *( (char*) memchr(danger, 'x', 100) );

#else
#   error "didn't select test"
    ERROR ERROR ERROR;
#endif

    printf(safe);
}
