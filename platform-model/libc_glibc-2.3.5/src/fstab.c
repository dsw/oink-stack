#include <prelude.h>
#include <fstab.h>

// struct fstab
// {
//     char *fs_spec;
//     char *fs_file;
//     char *fs_vfstype;
//     char *fs_mntops;
//     const char *fs_type;
//     int	fs_freq;
//     int	fs_passno;
// };

static inline struct fstab * __get_tainted_fstab()
{/*T:H*/
    struct fstab* p;
    __DO_TAINT(p->fs_spec);
    __DO_TAINT(p->fs_file);
    __DO_TAINT(p->fs_vfstype);
    __DO_TAINT(p->fs_mntops);
    return p;
}

struct fstab *getfsent (void) { return __get_tainted_fstab(); }
struct fstab *getfsspec (const char *name) { return __get_tainted_fstab(); }
struct fstab *getfsfile (const char *name) { return __get_tainted_fstab(); }
int setfsent (void) {}
void endfsent (void) {}
