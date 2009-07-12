#include <glibc-fs/prelude.h>
#include <fts.h>

// typedef struct {
//     struct _ftsent *fts_cur;
//     struct _ftsent *fts_child;
//     struct _ftsent **fts_array;
//     dev_t fts_dev;
//     char *fts_path;
//     int fts_rfd;
//     int fts_pathlen;
//     int fts_nitems;
//     int (*fts_compar) (const void *, const void *);

//     int fts_options;
// } FTS;

// typedef struct _ftsent {
//     struct _ftsent *fts_cycle;
//     struct _ftsent *fts_parent;
//     struct _ftsent *fts_link;
//     long fts_number;
//     void *fts_pointer;
//     char *fts_accpath;
//     char *fts_path;
//     int fts_errno;
//     int fts_symfd;
//     u_short fts_pathlen;
//     u_short fts_namelen;

//     ino_t fts_ino;
//     dev_t fts_dev;
//     nlink_t fts_nlink;

//     short fts_level;

//     u_short fts_info;

//     u_short fts_flags;

//     u_short fts_instr;

//     struct stat *fts_statp;
//     char fts_name[1];
// } FTSENT;

static inline void __taint_FTSENT(FTSENT * p)
{/*T:H*/
    __DO_TAINT(p->fts_accpath);
    __DO_TAINT(p->fts_path);
    __DO_TAINT(p->fts_name);
    p->fts_cycle = p;
    p->fts_parent = p;
    p->fts_link = p;
}

static inline FTSENT * __get_tainted_FTSENT(void)
{/*T:H*/
    FTSENT * p;
    __taint_FTSENT(p);
    return p;
}

static inline void __taint_FTS(FTS* p)
{/*T:H*/
    __DO_TAINT(p->fts_path);
    p->fts_cur = p->fts_child = p->fts_array[0] = __get_tainted_FTSENT();
}

FTSENT *fts_children (FTS * p, int i) { return p->fts_cur; }

int fts_close (FTS * p) {}

FTS *fts_open (char * const * path_argv, int i, int (*compar)(const FTSENT **, const FTSENT **))
{
    FTS* p;
    p->fts_compar = (int (*) (const void *, const void *)) compar;
    __taint_FTS(p);
    return p;
}

FTSENT *fts_read (FTS * p) { return p->fts_cur; }

int fts_set (FTS * pf, FTSENT * pe, int i) {
    __taint_FTS(pf);
    __taint_FTSENT(pe);
}
