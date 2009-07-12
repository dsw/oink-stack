#include <glibc-fs/prelude.h>
#include <mntent.h>

// struct mntent
// {
//     char *mnt_fsname;
//     char *mnt_dir;
//     char *mnt_type;
//     char *mnt_opts;
//     int mnt_freq;
//     int mnt_passno;
// };

struct mntent* __get_tainted_mntent() {
    struct mntent* p;
    __DO_TAINT(p->mnt_fsname);
    __DO_TAINT(p->mnt_dir);
    __DO_TAINT(p->mnt_type);
    __DO_TAINT(p->mnt_opts);
    return p;
}

FILE *setmntent (const char *file, const char *mode) {}

struct mntent *getmntent (FILE *stream) { return __get_tainted_mntent(); }

struct mntent *getmntent_r (FILE * stream,
                            struct mntent * result,
                            char $tainted * buffer,
                            int bufsize)
{ return __get_tainted_mntent(); }

int addmntent (FILE * stream, const struct mntent * mnt) {}

int endmntent (FILE *stream) {}

char $tainted *hasmntopt (const struct mntent *mnt, const char *opt) {}
