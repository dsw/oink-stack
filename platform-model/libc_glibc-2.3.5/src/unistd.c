#include <glibc-fs/prelude.h>
#include <unistd.h>

int access (const char *name, int type) {}

int euidaccess (const char *name, int type) {}

__off_t lseek (int fd, __off_t offset, int whence) {}
__off64_t lseek64 (int fd, __off64_t offset, int whence) {}

int close (int fd) {}

ssize_t read (int fd, void $tainted *buf, size_t nbytes) {}
ssize_t __read (int fd, void $tainted *buf, size_t nbytes) {}

ssize_t write (int fd, const void *buf, size_t n) {}

ssize_t pread (int fd, void $tainted *buf, size_t nbytes, __off_t offset) {}

ssize_t pwrite (int fd, const void *buf, size_t n, __off_t offset) {}

ssize_t pread64 (int fd, void $tainted *buf, size_t nbytes, __off64_t offset) {}

ssize_t pwrite64 (int fd, const void *buf, size_t n, __off64_t offset) {}

int pipe (int pipedes[2]) {}

unsigned int alarm (unsigned int seconds) {}

unsigned int sleep (unsigned int seconds) {}

__useconds_t ualarm (__useconds_t value, __useconds_t interval) {}

int usleep (__useconds_t useconds) {}

int pause (void) {}

int chown (const char *file, __uid_t owner, __gid_t group) {}

int fchown (int fd, __uid_t owner, __gid_t group) {}

int lchown (const char *file, __uid_t owner, __gid_t group) {}

int chdir (const char *path) {}

int fchdir (int fd) {}

char $tainted *getcwd (char $tainted *buf, size_t size) {}

char $tainted *get_current_dir_name (void) {}

char $tainted *getwd (char $tainted *buf) {}

int dup (int fd) {}

int dup2 (int fd, int fd2) {}

// char **environ;

// I don't think exec* should modify argv/envp, but they're not marked as
// const, so be safe for now.
int execve (const char *path, char $tainted *const argv[], char $tainted *const envp[]) {}

int fexecve (int fd, char $tainted *const argv[], char $tainted *const envp[]) {}

int execv (const char *path, char $tainted *const argv[]) {}

int execle (const char *path, const char *arg, ...) {}

int execl (const char *path, const char *arg, ...) {}

int execvp (const char *file, char $tainted *const argv[]) {}

int execlp (const char *file, const char *arg, ...) {}

int nice (int inc) {}

void _exit (int status) { __void_noreturn(); }

long int pathconf (const char *path, int name) {}

long int fpathconf (int fd, int name) {}

long int sysconf (int name) {}

size_t confstr (int name, char $tainted *buf, size_t len) {}

__pid_t getpid (void) {}

__pid_t getppid (void) {}

__pid_t getpgrp (void) {}

__pid_t __getpgid (__pid_t pid) {}
__pid_t getpgid (__pid_t pid) {}

int setpgid (__pid_t pid, __pid_t pgid) {}

int setpgrp (void) {}

__pid_t setsid (void) {}

__pid_t getsid (__pid_t pid) {}

__uid_t getuid (void) {}

__uid_t geteuid (void) {}

__gid_t getgid (void) {}

__gid_t getegid (void) {}

int getgroups (int size, __gid_t list[]) {}

int group_member (__gid_t gid) {}

int setuid (__uid_t uid) {}

int setreuid (__uid_t ruid, __uid_t euid) {}

int seteuid (__uid_t uid) {}

int setgid (__gid_t gid) {}

int setregid (__gid_t rgid, __gid_t egid) {}

int setegid (__gid_t gid) {}

int getresuid (__uid_t *euid, __uid_t *ruid, __uid_t *suid) {}

int getresgid (__gid_t *egid, __gid_t *rgid, __gid_t *sgid) {}

int setresuid (__uid_t euid, __uid_t ruid, __uid_t suid) {}

int setresgid (__gid_t egid, __gid_t rgid, __gid_t sgid) {}

__pid_t fork (void) {}

__pid_t vfork (void) {}

char $tainted *ttyname (int fd) {}

int ttyname_r (int fd, char $tainted *buf, size_t buflen) {}

int isatty (int fd) {}

int ttyslot (void) {}

int link (const char *from, const char *to) {}

int symlink (const char *from, const char *to) {}

int readlink (const char * path, char $tainted * buf, size_t len) {}

int unlink (const char *name) {}

int rmdir (const char *path) {}

__pid_t tcgetpgrp (int fd) {}

int tcsetpgrp (int fd, __pid_t pgrp_id) {}

char $tainted *getlogin (void) {}

int getlogin_r (char $tainted *name, size_t name_len) {}

int setlogin (const char *name) {}

int gethostname (char $tainted *name, size_t len) {}

int sethostname (const char *name, size_t len) {}

int sethostid (long int id) {}

int getdomainname (char $tainted *name, size_t len) {}
int setdomainname (const char *name, size_t len) {}

int vhangup (void) {}

int revoke (const char *file) {}

int profil (unsigned short int *sample_buffer, size_t size,
            size_t offset, unsigned int scale) {}

//// acct.c
// int acct (const char *name) {}

char $tainted *getusershell (void) {}
void endusershell (void) {}
void setusershell (void) {}

int daemon (int nochdir, int noclose) {}

int chroot (const char *path) {}

char $tainted *getpass (const char *prompt) {}

int fsync (int fd) {}

long int gethostid (void) {}

void sync (void) {}

int getpagesize (void) {}

int truncate (const char *file, __off_t length) {}
int truncate64 (const char *file, __off64_t length) {}

int ftruncate (int fd, __off_t length) {}
int ftruncate64 (int fd, __off64_t length) {}

int getdtablesize (void) {}

int brk (void *addr) {}

void *sbrk (intptr_t delta) {}

long int syscall (long int sysno, ... $tainted) {}

//// These are in fcntl.c
// int lockf (int fd, int cmd, __off_t len) {}
// int lockf64 (int fd, int cmd, __off64_t len) {}

int fdatasync (int fildes) {}

//// These are in crypt.c
// char $tainted *crypt (const char *key, const char *salt) {}
// void encrypt (char $tainted *block, int edflag) {}

void swab (const void $_1 * from, void $_1_2 * to, ssize_t n) {}

//// This is in stdio.c
// char $tainted *ctermid (char $tainted *s) {}
