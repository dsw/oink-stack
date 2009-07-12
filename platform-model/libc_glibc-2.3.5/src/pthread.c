
// pthreads:
//   - We assume integer data such as thread properties are not tainted.
//   - We assume there is no tainted information in synchronization (e.g.
//     taint could be laundered through condition variables).
//   - We flow all return values together.

#include <glibc-fs/prelude.h>
#include <pthread.h>

// struct _pthread_cleanup_buffer
// {
//     void (*__routine) (void *);
//     void *arg;
//     int canceltype;
//     struct _pthread_cleanup_buffer *prev;
// };

// // custom struct (not from glibc)
// struct pthread_data {
//     void *retval;
// };

// We flow all return values through this global.
static void *global_retval;
// static pthread_data this_thread;

int pthread_create (pthread_t * threadp, const pthread_attr_t * attr, void *(*start_routine) (void *), void * arg)
{
    // (*((struct pthread_data**) threadp))->retval = start_routine(arg);
    global_retval = start_routine(arg);
}

pthread_t pthread_self (void) {
    // return this_thread;
}

int pthread_equal (pthread_t thread1, pthread_t thread2) {}

extern void exit(int x);
void pthread_exit (void *retval) //__attribute__ ((__noreturn__))
{
    global_retval = retval;
    exit(0); // make attribute((noreturn)) happy
}

int pthread_join (pthread_t th, void **thread_return) {
    *thread_return = global_retval;
}

int pthread_detach (pthread_t th) {}

int pthread_attr_init (pthread_attr_t *attr) {}

int pthread_attr_destroy (pthread_attr_t *attr) {}

int pthread_attr_setdetachstate (pthread_attr_t *attr, int detachstate) {}

int pthread_attr_getdetachstate (const pthread_attr_t *attr, int *detachstate) {}

int pthread_attr_setschedparam (pthread_attr_t * attr, const struct sched_param * param) {}

int pthread_attr_getschedparam (const pthread_attr_t * attr, struct sched_param * param) {}

int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy) {}

int pthread_attr_getschedpolicy (const pthread_attr_t * attr, int * policy) {}

int pthread_attr_setinheritsched (pthread_attr_t *attr, int inherit) {}

int pthread_attr_getinheritsched (const pthread_attr_t * attr, int * inherit) {}

int pthread_attr_setscope (pthread_attr_t *attr, int scope) {}

int pthread_attr_getscope (const pthread_attr_t * attr, int * scope) {}

int pthread_attr_setguardsize (pthread_attr_t *attr, size_t guardsize) {}

int pthread_attr_getguardsize (const pthread_attr_t * attr, size_t * guardsize) {}

int pthread_attr_setstackaddr (pthread_attr_t *attr, void *stackaddr) {}

int pthread_attr_getstackaddr (const pthread_attr_t * attr, void ** stackaddr) {}

int pthread_attr_setstack (pthread_attr_t *attr, void *stackaddr, size_t stacksize) {}

int pthread_attr_getstack (const pthread_attr_t * attr, void ** stackaddr, size_t * stacksize) {}

int pthread_attr_setstacksize (pthread_attr_t *attr, size_t stacksize) {}

int pthread_attr_getstacksize (const pthread_attr_t * attr, size_t * stacksize) {}

int pthread_getattr_np (pthread_t th, pthread_attr_t *attr) {}

int pthread_setschedparam (pthread_t target_thread, int policy, const struct sched_param *param) {}

int pthread_getschedparam (pthread_t target_thread, int * policy, struct sched_param * param) {}

int pthread_getconcurrency (void) {}

int pthread_setconcurrency (int level) {}

int pthread_yield (void) {}

int pthread_mutex_init (pthread_mutex_t * mutex, const pthread_mutexattr_t * mutex_attr) {}

int pthread_mutex_destroy (pthread_mutex_t *mutex) {}

int pthread_mutex_trylock (pthread_mutex_t *mutex) {}

int pthread_mutex_lock (pthread_mutex_t *mutex) {}

int pthread_mutex_timedlock (pthread_mutex_t * mutex, const struct timespec * abstime) {}

int pthread_mutex_unlock (pthread_mutex_t *mutex) {}

int pthread_mutexattr_init (pthread_mutexattr_t *attr) {}

int pthread_mutexattr_destroy (pthread_mutexattr_t *attr) {}

int pthread_mutexattr_getpshared (const pthread_mutexattr_t * attr, int * pshared) {}

int pthread_mutexattr_setpshared (pthread_mutexattr_t *attr, int pshared) {}

int pthread_mutexattr_settype (pthread_mutexattr_t *attr, int kind) {}

int pthread_mutexattr_gettype (const pthread_mutexattr_t * attr, int * kind) {}

int pthread_cond_init (pthread_cond_t * cond, const pthread_condattr_t * cond_attr) {}

int pthread_cond_destroy (pthread_cond_t *cond) {}

int pthread_cond_signal (pthread_cond_t *cond) {}

int pthread_cond_broadcast (pthread_cond_t *cond) {}

int pthread_cond_wait (pthread_cond_t * cond, pthread_mutex_t * mutex) {}

int pthread_cond_timedwait (pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec * abstime) {}

int pthread_condattr_init (pthread_condattr_t *attr) {}

int pthread_condattr_destroy (pthread_condattr_t *attr) {}

int pthread_condattr_getpshared (const pthread_condattr_t * attr, int * pshared) {}

int pthread_condattr_setpshared (pthread_condattr_t *attr, int pshared) {}

int pthread_rwlock_init (pthread_rwlock_t * rwlock, const pthread_rwlockattr_t * attr) {}

int pthread_rwlock_destroy (pthread_rwlock_t *rwlock) {}

int pthread_rwlock_rdlock (pthread_rwlock_t *rwlock) {}

int pthread_rwlock_tryrdlock (pthread_rwlock_t *rwlock) {}

int pthread_rwlock_timedrdlock (pthread_rwlock_t * rwlock, const struct timespec * abstime) {}

int pthread_rwlock_wrlock (pthread_rwlock_t *rwlock) {}

int pthread_rwlock_trywrlock (pthread_rwlock_t *rwlock) {}

int pthread_rwlock_timedwrlock (pthread_rwlock_t * rwlock, const struct timespec * abstime) {}

int pthread_rwlock_unlock (pthread_rwlock_t *rwlock) {}

int pthread_rwlockattr_init (pthread_rwlockattr_t *attr) {}

int pthread_rwlockattr_destroy (pthread_rwlockattr_t *attr) {}

int pthread_rwlockattr_getpshared (const pthread_rwlockattr_t * attr, int * pshared) {}

int pthread_rwlockattr_setpshared (pthread_rwlockattr_t *attr, int pshared) {}

int pthread_rwlockattr_getkind_np (const pthread_rwlockattr_t *attr, int *pref) {}

int pthread_rwlockattr_setkind_np (pthread_rwlockattr_t *attr, int pref) {}

int pthread_spin_init (pthread_spinlock_t *lock, int pshared) {}

int pthread_spin_destroy (pthread_spinlock_t *lock) {}

int pthread_spin_lock (pthread_spinlock_t *lock) {}

int pthread_spin_trylock (pthread_spinlock_t *lock) {}

int pthread_spin_unlock (pthread_spinlock_t *lock) {}

int pthread_barrier_init (pthread_barrier_t * barrier, const pthread_barrierattr_t * attr, unsigned int count) {}

int pthread_barrier_destroy (pthread_barrier_t *barrier) {}

int pthread_barrierattr_init (pthread_barrierattr_t *attr) {}

int pthread_barrierattr_destroy (pthread_barrierattr_t *attr) {}

int pthread_barrierattr_getpshared (const pthread_barrierattr_t * __attr, int * pshared) {}

int pthread_barrierattr_setpshared (pthread_barrierattr_t *attr, int pshared) {}

int pthread_barrier_wait (pthread_barrier_t *barrier) {}

int pthread_key_create (pthread_key_t *key, void (*destr_function) (void *)) {
    destr_function(key);
}

int pthread_key_delete (pthread_key_t key) {}

int pthread_setspecific (pthread_key_t key, const void *pointer) {}

void *pthread_getspecific (pthread_key_t key) {}

int pthread_once (pthread_once_t *once_control, void (*init_routine) (void)) {
    init_routine();
}

int pthread_setcancelstate (int state, int *oldstate) {}

int pthread_setcanceltype (int type, int *oldtype) {}

int pthread_cancel (pthread_t cancelthread) {}

void pthread_testcancel (void) {}

void _pthread_cleanup_push (struct _pthread_cleanup_buffer *buffer, void (*routine) (void *), void *arg) {
    routine(arg);
}

void _pthread_cleanup_pop (struct _pthread_cleanup_buffer *buffer, int execute) {}

void _pthread_cleanup_push_defer (struct _pthread_cleanup_buffer *buffer, void (*routine) (void *), void *arg) {
    routine(arg);
}

void _pthread_cleanup_pop_restore (struct _pthread_cleanup_buffer *buffer, int execute) {}

int pthread_getcpuclockid (pthread_t thread_id, clockid_t *clock_id) {}

int pthread_atfork (void (*prepare) (void), void (*parent) (void), void (*child) (void)) {
    prepare();
    parent();
    child();
}

void pthread_kill_other_threads_np (void) {}

// obsolete functions that do (basically) nothing
int pthread_mutexattr_setkind_np(pthread_mutexattr_t *attr, int kind) {}

int pthread_mutexattr_getkind_np(const pthread_mutexattr_t *attr, int *kind) {}
