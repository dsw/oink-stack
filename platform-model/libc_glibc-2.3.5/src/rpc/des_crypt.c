// $Id$

#include <glibc-fs/prelude.h>
#include <rpc/des_crypt.h>

int cbc_crypt (char /*const*/ *key, char $tainted *buf, unsigned len, unsigned mode, char $tainted *ivec) {}

int ecb_crypt (char /*const*/ *key, char $tainted *buf, unsigned len, unsigned mode) {}

void des_setparity (char $tainted *key) {}
