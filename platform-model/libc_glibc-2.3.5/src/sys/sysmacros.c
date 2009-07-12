#include <glibc-fs/prelude.h>

// #define __inline /*nothing*/
// #include <sys/sysmacros.h>

unsigned int gnu_dev_major (unsigned long long int dev) { return dev; }

unsigned int gnu_dev_minor (unsigned long long int dev) { return dev; }

unsigned long long int gnu_dev_makedev (unsigned int major, unsigned int minor) { return major+minor; }
