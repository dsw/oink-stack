#ifndef PRELUDE_CXX_H
#define PRELUDE_CXX_H

#include <bits/c++config.h>

// quarl 2006-06-16
//    Undefine this so that we get extra template definitions (i.e. bits/*.tcc
//    files)

#undef _GLIBCXX_EXPORT_TEMPLATE

// Elsa bug: doesn't work in g++ mode
#define __restrict

#endif
