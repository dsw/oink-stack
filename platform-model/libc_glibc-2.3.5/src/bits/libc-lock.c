#ifndef GLIBCFS_IGNORE_INCOMPLETE
// this file is actually complete, but we don't want to include <pthread.h>

#include <glibc-fs/prelude.h>
#define NOT_IN_libc
#include <bits/libc-lock.h>

int __pthread_mutex_init (pthread_mutex_t *mutex, const pthread_mutexattr_t *mutex_attr) {}

int __pthread_mutex_destroy (pthread_mutex_t *mutex) {}

int __pthread_mutex_trylock (pthread_mutex_t *mutex) {}

int __pthread_mutex_lock (pthread_mutex_t *mutex) {}

int __pthread_mutex_unlock (pthread_mutex_t *mutex) {}

int __pthread_mutexattr_init (pthread_mutexattr_t *attr) {}

int __pthread_mutexattr_destroy (pthread_mutexattr_t *attr) {}

int __pthread_mutexattr_settype (pthread_mutexattr_t *attr, int kind) {}

int __pthread_rwlock_init (pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) {}

int __pthread_rwlock_destroy (pthread_rwlock_t *rwlock) {}

int __pthread_rwlock_rdlock (pthread_rwlock_t *rwlock) {}

int __pthread_rwlock_tryrdlock (pthread_rwlock_t *rwlock) {}

int __pthread_rwlock_wrlock (pthread_rwlock_t *rwlock) {}

int __pthread_rwlock_trywrlock (pthread_rwlock_t *rwlock) {}

int __pthread_rwlock_unlock (pthread_rwlock_t *rwlock) {}

int __pthread_key_create (pthread_key_t *key, void (*__destr_function) (void *)) {}

int __pthread_setspecific (pthread_key_t key, const void *pointer) {}

void *__pthread_getspecific (pthread_key_t key) {}

int __pthread_once (pthread_once_t *once_control, void (*__init_routine) (void)) {}

int __pthread_atfork (void (*__prepare) (void), void (*__parent) (void), void (*__child) (void)) {}

#endif
