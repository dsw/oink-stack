// #ifndef GLIBCFS_IGNORE_INCOMPLETE

#include <glibc-fs/prelude.h>
#include <sys/shm.h>

int shmctl (int shmid, int cmd, struct shmid_ds *buf) {}

int shmget (key_t key, size_t size, int shmflg) {}

// Shared memory: for now, just make all shared memory tainted.

void $tainted *shmat (int shmid, const void $tainted *shmaddr, int shmflg)
{
    // void* p = (void*) shmaddr;
    // __DO_TAINT(p);
}

// void *shmat (int shmid, void $tainted *shmaddr, int shmflg) {}

int shmdt (const void $tainted *shmaddr)
{
    // void* p = (void*) shmaddr;
    // __DO_TAINT(p);
}

// int shmdt (void $tainted *shmaddr) {}

// #endif
