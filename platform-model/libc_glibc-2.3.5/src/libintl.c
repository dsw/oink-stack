#include <glibc-fs/prelude.h>
#include <libintl.h>

// *** We assume that the translation library is trusted. ***

FAKEBUILTIN
char $_1_2 *gettext (const char $_1 *msgid) {}

FAKEBUILTIN
char $_1_2 *dgettext (const char *domainname, const char $_1 *msgid) {}

char $_1_2 *__dgettext (const char *domainname, const char $_1 *msgid) {}

FAKEBUILTIN
char $_1_2 *dcgettext (const char *domainname, __const char $_1 *msgid, int category) {}
char $_1_2 *__dcgettext (const char *domainname, const char $_1 *msgid, int category) {}

char $_1_2_3 *ngettext (const char $_1 *msgid1, const char $_2 *msgid2, unsigned long int n) {}

char $_1_2_3 *dngettext (const char *domainname, const char $_1 *msgid1, __const char $_2 *msgid2, unsigned long int n) {}

char $_1_2_3 *dcngettext (const char *domainname, const char $_1 *msgid1, const char $_2 *msgid2, unsigned long int n, int category) {}

char $tainted *textdomain (const char *domainname) {}

char $tainted *bindtextdomain (const char *domainname, const char *dirname) {}

char $tainted *bind_textdomain_codeset (const char *domainname, const char *codeset) {}
