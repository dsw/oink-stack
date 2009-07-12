// dlopen et al - this is currently unimplemented, i.e. unsound.
//
// We provide dummy definitions just so we can link.
//
// If this function is used, you should get a warning via the unsound-if-used
// control.

//#ifndef GLIBCFS_IGNORE_INCOMPLETE

// $Id$

#include <glibc-fs/prelude.h>
#include <dlfcn.h>

// XXX
void $tainted *dlopen (const char *file, int mode) {}

int dlclose (void *handle) {}

// XXX
void $tainted *dlsym (void * handle, const char * name) {}

// XXX
void $tainted *dlvsym (void * handle, const char * name, const char * version) {}

char $tainted *dlerror (void) {}

// typedef struct
// {
//     const char *dli_fname;
//     void *dli_fbase;
//     const char *dli_sname;
//     void *dli_saddr;
// } Dl_info;

static inline void __taint_Dl_info(Dl_info* p)
{/*T:H*/
    __DO_TAINT(p->dli_fname);
    __DO_TAINT(p->dli_sname);
}

int dladdr (const void *address, Dl_info *info) {
    __taint_Dl_info(info);
}

// undocumented:
//  int dladdr1 (const void *address, Dl_info *__info, void **extra_info, int flags);

// undocumented:
//  int dlinfo (void * handle, int request, void * arg);

// typedef struct
// {
//     char *dls_name;
//     unsigned int dls_flags;
// } Dl_serpath;

// typedef struct
// {
//     size_t dls_size;
//     unsigned int dls_cnt;
//     Dl_serpath dls_serpath[1];
// } Dl_serinfo;
