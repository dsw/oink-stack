#include <glibc-fs/prelude.h>
#include <ttyent.h>

// struct ttyent {
//     char        *ty_name;
//     char        *ty_getty;
//     char        *ty_type;

//     int ty_status;
//     char        *ty_window;
//     char        *ty_comment;
// };

static inline struct ttyent* __get_tainted_ttyent(void)
{/*T:H*/
    struct ttyent* p;
    __DO_TAINT(p->ty_name);
    __DO_TAINT(p->ty_getty);
    __DO_TAINT(p->ty_type);
    __DO_TAINT(p->ty_window);
    __DO_TAINT(p->ty_comment);
    return p;
}

struct ttyent *getttyent (void) { return __get_tainted_ttyent(); }
struct ttyent *getttynam (const char *tty) { return __get_tainted_ttyent(); }
int setttyent (void) {}
int endttyent (void) {}
