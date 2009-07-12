#include <glibc-fs/prelude.h>
#include <semaphore.h>

// typedef struct
// {
//     struct _pthread_fastlock sem_lock;
//     int sem_value;
//     _pthread_descr sem_waiting;
// } sem_t;

int sem_init (sem_t *sem, int pshared, unsigned int value) {}

int sem_destroy (sem_t *sem) {}

sem_t *sem_open (const char *name, int oflag, ... $tainted) {
    va_list arg;
    va_start(arg, oflag);
    __taint_vararg_environment(arg);
    va_end(arg);
}

int sem_close (sem_t *sem) {}

int sem_unlink (const char *name) {}

int sem_wait (sem_t *sem) {}

int sem_timedwait (sem_t * sem, const struct timespec * abstime) {}

int sem_trywait (sem_t *sem) {}

int sem_post (sem_t *sem) {}

int sem_getvalue (sem_t * sem, int * sval) {}
