//@@ LDFLAGS: -ldl

#include <dlfcn.h>

int main()
{
    // TODO placeholder
    "EXPECT_NO_WARNINGS";

    // in the short-term, we should at least get an unsoundness warning

    "EXPECT_UNSOUNDNESS_WARNINGS";
    dlopen("x",0);
}
