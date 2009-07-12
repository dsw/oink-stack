#include <glibc-fs/prelude.h>
#include <aio.h>

// struct aiocb
// {
//     int aio_fildes;
//     int aio_lio_opcode;
//     int aio_reqprio;
//     volatile void *aio_buf;
//     size_t aio_nbytes;
//     struct sigevent aio_sigevent;

//     struct aiocb *next_prio;
//     int abs_prio;
//     int policy;
//     int error_code;
//     __ssize_t return_value;

//     __off_t aio_offset;
//     char pad[sizeof (__off64_t) - sizeof (__off_t)];
//     char unused[32];
// };

// struct aiocb64
// {
//     int aio_fildes;
//     int aio_lio_opcode;
//     int aio_reqprio;
//     volatile void *aio_buf;
//     size_t aio_nbytes;
//     struct sigevent aio_sigevent;

//     struct aiocb *next_prio;
//     int abs_prio;
//     int policy;
//     int error_code;
//     __ssize_t return_value;

//     __off64_t aio_offset;
//     char unused[32];
// };

void aio_init (const struct aioinit *init) {}

static inline void __taint_aiocb(struct aiocb * p)
{/*T:H*/
    __DO_TAINT(p->aio_buf);
    p->__next_prio = p;
}

static inline void __taint_aiocb64(struct aiocb64 * p)
{/*T:H*/
    __DO_TAINT(p->aio_buf);
    // why is it an aiocb rather than aiocb64?
    p->__next_prio = (struct aiocb*) p;
}

int aio_read (struct aiocb *aiocbp) { __taint_aiocb(aiocbp); }

int aio_write (struct aiocb *aiocbp) { __taint_aiocb(aiocbp); }

int lio_listio (int mode, struct aiocb *const list[], int nent, struct sigevent * sig)
{ __taint_aiocb(list[0]); }

int aio_error (const struct aiocb *aiocbp) {}

__ssize_t aio_return (struct aiocb *aiocbp) { __taint_aiocb(aiocbp); }

int aio_cancel (int fildes, struct aiocb *aiocbp) { __taint_aiocb(aiocbp); }

int aio_suspend (const struct aiocb *const list[], int nent,
                 const struct timespec * timeout) {}

int aio_fsync (int operation, struct aiocb *aiocbp)
{ __taint_aiocb(aiocbp); }

int aio_read64 (struct aiocb64 *aiocbp) { __taint_aiocb64(aiocbp); }
int aio_write64 (struct aiocb64 *aiocbp) { __taint_aiocb64(aiocbp); }

int lio_listio64 (int mode, struct aiocb64 *const list[], int nent, struct sigevent * sig)
{ __taint_aiocb64(list[0]); }

int aio_error64 (const struct aiocb64 *aiocbp) {}
__ssize_t aio_return64 (struct aiocb64 *aiocbp) { __taint_aiocb64(aiocbp); }

int aio_cancel64 (int fildes, struct aiocb64 *aiocbp) { __taint_aiocb64(aiocbp); }

int aio_suspend64 (const struct aiocb64 *const list[], int nent,
                   const struct timespec * timeout)
{}

int aio_fsync64 (int operation, struct aiocb64 *aiocbp)
{ __taint_aiocb64(aiocbp); }
