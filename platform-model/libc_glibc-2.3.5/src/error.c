#include <glibc-fs/prelude.h>
#include <error.h>

void error (int status, int errnum, const char $untainted *format, ...) {}

void error_at_line (int status, int errnum, const char *fname,
                    unsigned int lineno, const char $untainted *format, ...) {}
