#include <glibc-fs/prelude.h>
#include <termios.h>

// struct termios
// {
//     tcflag_t c_iflag;                               /* input mode flags */
//     tcflag_t c_oflag;                               /* output mode flags */
//     tcflag_t c_cflag;                               /* control mode flags */
//     tcflag_t c_lflag;                               /* local mode flags */
//     cc_t c_line;                                    /* line discipline */
//     cc_t c_cc[32];                                  /* control characters */
//     speed_t c_ispeed;                               /* input speed */
//     speed_t c_ospeed;                               /* output speed */
// };

speed_t cfgetospeed (const struct termios *termios_p) {}

speed_t cfgetispeed (const struct termios *termios_p) {}

int cfsetospeed (struct termios *termios_p, speed_t speed) {}

int cfsetispeed (struct termios *termios_p, speed_t speed) {}

int cfsetspeed (struct termios *termios_p, speed_t speed) {}

int tcgetattr (int fd, struct termios *termios_p) {}

int tcsetattr (int fd, int optional_actions, const struct termios *termios_p) {}

void cfmakeraw (struct termios *termios_p) {}

int tcsendbreak (int fd, int duration) {}

int tcdrain (int fd) {}

int tcflush (int fd, int queue_selector) {}

int tcflow (int fd, int action) {}

__pid_t tcgetsid (int fd) {}
