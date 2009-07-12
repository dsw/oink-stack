// $Id$

#include <glibc-fs/prelude.h>

// call 'main' with tainted arguments.

// extern int main(int $tainted argc, char $tainted * * argv,
//                 char $tainted * * environ);

// Don't annotate "main" with qualifiers, otherwise Oink will consider it
// defined.  __glibc_prelude_crt0 will make sure we consider its arguments
// tainted.

extern int main(int argc, char * * argv, char * * environ);


int __glibc_prelude_crt0()
{
    int $tainted argc;
    char $tainted * * argv;
    char $tainted * * environ;
    return main(argc, argv, environ);
}

int $tainted __argc;
char $tainted * * __argv;
char $tainted * * __environ;
char $tainted * * environ;
// char * * environ = __environ;

typedef int (*intfunc_t)();
intfunc_t __glibc_prelude_call_crt0()
{
    // take address to force oink to not elide __glibc_prelude_crt0.

    return & __glibc_prelude_crt0;
}
