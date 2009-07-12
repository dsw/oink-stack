#include <alloca.h>

// just test linking.

int main()
{
    "EXPECT_NO_WARNINGS";
    alloca(42);
    __builtin_alloca(43);
}

