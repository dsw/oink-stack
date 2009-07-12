#include <glibc-fs/prelude.h>
#include <iconv.h>

iconv_t iconv_open (const char *tocode, const char *fromcode) {}

size_t iconv (iconv_t cd, char $_1 ** inbuf, size_t * inbytesleft, char $_1_2 ** outbuf, size_t * outbytesleft) {}

int iconv_close (iconv_t cd) {}
