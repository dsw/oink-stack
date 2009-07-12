#include <glibc-fs/prelude.h>
#include <langinfo.h>

char $tainted *nl_langinfo (nl_item item) {}

char $tainted *nl_langinfo_l (nl_item item, __locale_t l) {}
