#include <glibc-fs/prelude.h>
#include <nl_types.h>

nl_catd catopen (const char *cat_name, int flag) {}

// translation services.

// quarl 2006-06-15
//    Taint return value from 'string'.  This assumes, like for gettext, that
//    the catalog string's format string matches the "default value"'s string
//    (which may be more likely to be incorrect since the input is a catalog
//    index rather than the default value).

// STRING is the default value if message can't be found.
char $_1_2 *catgets (nl_catd catalog, int set, int number, const char $_1 *string) {}

int catclose (nl_catd catalog) {}
