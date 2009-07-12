#include <stdio.h>
#include <stdlib.h>

int main()
{
    char* s;
    s = getenv("HOME");
    printf(s);

    "EXPECT_TAINT_WARNINGS";
}
