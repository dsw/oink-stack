#include <glibc-fs/prelude.h>
#include <envz.h>

char *envz_entry (const char * envz, size_t envz_len, const char * name) {
    return (char*) envz;
}

char *envz_get (const char * envz, size_t envz_len, const char * name) {
    return (char*) envz;
}

error_t envz_add (char ** envz, size_t * envz_len, const char * name, const char * value) {
    **envz = *name;
    **envz = *value;
}

error_t envz_merge (char ** envz, size_t * envz_len, const char * envz2, size_t envz2_len, int override) {
    **envz = *envz2;
}

void envz_remove (char ** envz, size_t * envz_len, const char * name) {
    **envz = *name;
}

void envz_strip (char ** envz, size_t * envz_len) {}
