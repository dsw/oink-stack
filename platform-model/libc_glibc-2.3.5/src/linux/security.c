// not in glibc; actually in linux, but convenient to put here.

#include <glibc-fs/prelude.h>
// #include <linux/security.h>
#include <linux/capability.h>

int capset(cap_user_header_t header, cap_user_data_t data) {}

int capget(cap_user_header_t header, const cap_user_data_t data) {}
