#include <glibc-fs/prelude.h>
#include <argz.h>

error_t __argz_create (char $_1 *const __argv[], char $_1_2 ** argz, size_t * len) {}
error_t argz_create (char $_1 *const __argv[], char $_1_2 ** argz, size_t * len) {}

error_t __argz_create_sep (const char $_1 * string, int sep, char $_1_2 ** argz, size_t * len) {}
error_t argz_create_sep (const char $_1 * string, int sep, char $_1_2 ** argz, size_t * len) {}

size_t __argz_count (const char *argz, size_t len) {}
size_t argz_count (const char *argz, size_t len) {}

void __argz_extract (const char $_1 * argz, size_t len, char $_1_2 ** argv) {}
void argz_extract (const char $_1 * argz, size_t len, char $_1_2 ** argv) {}

void __argz_stringify (char *argz, size_t len, int sep) {}
void argz_stringify (char *argz, size_t len, int sep) {}

error_t __argz_append (char $_1_2 ** argz, size_t * argz_len, const char $_1 * buf, size_t _buf_len) {}
error_t argz_append (char $_1_2 ** argz, size_t * argz_len, const char $_1 * buf, size_t buf_len) {}

error_t __argz_add (char $_1_2 ** argz, size_t * argz_len, const char $_1 * str) {}
error_t argz_add (char $_1_2 ** argz, size_t * argz_len, const char $_1 * str) {}

error_t __argz_add_sep (char $_1_2 ** argz, size_t * argz_len, const char $_1 * string, int delim) {}
error_t argz_add_sep (char $_1_2 ** argz, size_t * argz_len, const char $_1 * string, int delim) {}

void __argz_delete (char ** argz, size_t * argz_len, char * entry) {}
void argz_delete (char ** argz, size_t * argz_len, char * entry) {}

error_t __argz_insert (char $_1_2 ** argz, size_t * argz_len, char $_1_2 * before, const char $_1 * entry) {}
error_t argz_insert (char $_1_2 ** argz, size_t * argz_len, char $_1_2 * before, const char $_1 * entry) {}

error_t __argz_replace (char $_1_2 ** argz, size_t * argz_len, const char * str, const char $_1 * with, unsigned int * replace_count) {}
error_t argz_replace (char $_1_2 ** argz, size_t * argz_len, const char * str, const char $_1 * with, unsigned int * replace_count) {}

char $_1_2 *__argz_next (const char $_1 * argz, size_t argz_len, const char * entry) {}
char $_1_2 *argz_next (const char $_1 * argz, size_t argz_len, const char * entry) {}
