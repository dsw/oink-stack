#include <glibc-fs/prelude.h>
#include <sys/sem.h>

int semctl (int semid, int semnum, int cmd, ...) {}

int semget (key_t key, int nsems, int semflg) {}

int semop (int semid, struct sembuf *sops, size_t nsops) {}

int semtimedop (int semid, struct sembuf *sops, size_t nsops,
                const struct timespec *timeout) {}
