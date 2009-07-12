#include <sys/ioctl.h>
#include <stdio.h>

int main()
{
    char *s;
    ioctl(0, 0, 0, s);
    printf(s);

    "EXPECT_TAINT_WARNINGS";
}
