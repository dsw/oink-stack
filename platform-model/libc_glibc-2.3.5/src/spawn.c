#include <glibc-fs/prelude.h>
#include <spawn.h>

// since argv/envp aren't const, $taint them

int posix_spawn (pid_t * pid,
                 const char * path,
                 const posix_spawn_file_actions_t * __file_actions,
                 const posix_spawnattr_t * attrp,
                 char $tainted *const argv[],
                 char $tainted *const envp[]) {}

int posix_spawnp (pid_t *pid, const char *file,
                  const posix_spawn_file_actions_t *file_actions,
                  const posix_spawnattr_t *attrp,
                  char $tainted *const argv[],
                  char $tainted *const envp[]) {}

int posix_spawnattr_init (posix_spawnattr_t *attr) {}

int posix_spawnattr_destroy (posix_spawnattr_t *attr) {}

int posix_spawnattr_getsigdefault (const posix_spawnattr_t * attr,
                                   sigset_t * sigdefault) {}

int posix_spawnattr_setsigdefault (posix_spawnattr_t * attr,
                                   const sigset_t * sigdefault) {}

int posix_spawnattr_getsigmask (const posix_spawnattr_t * attr,
                                sigset_t * sigmask) {}

int posix_spawnattr_setsigmask (posix_spawnattr_t * attr,
                                const sigset_t * sigmask) {}

int posix_spawnattr_getflags (const posix_spawnattr_t * attr,
                              short int * flags) {}

int posix_spawnattr_setflags (posix_spawnattr_t *_attr,
                              short int flags) {}

int posix_spawnattr_getpgroup (const posix_spawnattr_t * attr,
                               pid_t * pgroup) {}

int posix_spawnattr_setpgroup (posix_spawnattr_t *attr,
                               pid_t pgroup) {}

int posix_spawnattr_getschedpolicy (const posix_spawnattr_t * attr,
                                    int * schedpolicy) {}

int posix_spawnattr_setschedpolicy (posix_spawnattr_t *attr,
                                    int schedpolicy) {}

int posix_spawnattr_getschedparam (const posix_spawnattr_t * attr,
                                   struct sched_param * schedparam) {}

int posix_spawnattr_setschedparam (posix_spawnattr_t * attr,
                                   const struct sched_param * schedparam) {}

int posix_spawn_file_actions_init (posix_spawn_file_actions_t * file_actions) {}

int posix_spawn_file_actions_destroy (posix_spawn_file_actions_t * file_actions) {}

int posix_spawn_file_actions_addopen (posix_spawn_file_actions_t * file_actions,
                                      int fd, const char * path,
                                      int oflag, mode_t mode) {}

int posix_spawn_file_actions_addclose (posix_spawn_file_actions_t * file_actions,
                                       int fd) {}

int posix_spawn_file_actions_adddup2 (posix_spawn_file_actions_t * file_actions,
                                      int fd, int newfd) {}
