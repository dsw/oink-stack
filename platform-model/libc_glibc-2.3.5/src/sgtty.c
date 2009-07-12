#include <glibc-fs/prelude.h>
#include <sgtty.h>

int gtty (int fd, struct sgttyb *params) {}

int stty (int fd, const struct sgttyb *params) {}
