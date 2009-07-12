#include <glibc-fs/prelude.h>
#include <stropts.h>

// struct strbuf
// {
//     int maxlen;         /* Maximum buffer length.  */
//     int len;            /* Length of data.  */
//     char *buf;          /* Pointer to buffer.  */
// };

static inline void __taint_strbuf(struct strbuf *p)
{/*T:H*/
    __DO_TAINT(p->buf);
}

int isastream (int fildes) {}

int getmsg (int fildes, struct strbuf * ctlptr,
            struct strbuf * dataptr, int * flagsp)
{ __taint_strbuf(ctlptr);  __taint_strbuf(dataptr); }

int getpmsg (int fildes, struct strbuf * ctlptr,
             struct strbuf * dataptr, int * bandp, int * flagsp)
{ __taint_strbuf(ctlptr);  __taint_strbuf(dataptr); }

//// ioctl.c
// int ioctl (int fd, unsigned long int request, ...);

int putmsg (int fildes, const struct strbuf *ctlptr,
            const struct strbuf *dataptr, int flags) {}

int putpmsg (int fildes, const struct strbuf *ctlptr,
             const struct strbuf *dataptr, int band, int flags) {}

int fattach (int fildes, const char *path) {}

int fdetach (const char *path) {}
