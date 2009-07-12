#include <glibc-fs/prelude.h>
#include <sys/epoll.h>

// typedef union epoll_data
// {
//     void *ptr;
//     int fd;
//     uint32_t u32;
//     uint64_t u64;
// } epoll_data_t;

// struct epoll_event
// {
//     uint32_t events;
//     epoll_data_t data;
// };

static inline void __taint_epoll_event(struct epoll_event* p)
{/*T:H*/
    __DO_TAINT( p->data.ptr );
}

int epoll_create (int size) {}

int epoll_ctl (int epfd, int op, int fd, struct epoll_event *event)
{ __taint_epoll_event(event); }

int epoll_wait (int epfd, struct epoll_event *events, int maxevents, int timeout)
{ __taint_epoll_event(events); }
