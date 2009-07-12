// see License.txt for copyright and terms of use

// utilities for files that process command line flags

#ifndef CMD_UTIL_H
#define CMD_UTIL_H

// NOTE: please recall that two adjacent strings concatenate

#define HANDLE_FLAG(FLAG, PREFIX, FLAG_NAME) \
  do {                                                            \
    if (streq(arg, PREFIX FLAG_NAME)) {                           \
      shift(argc, argv);                                          \
      FLAG = true;                                                \
      return;                                                     \
    }                                                             \
    if (streq(arg, PREFIX "no-" FLAG_NAME)) {                     \
      shift(argc, argv);                                          \
      FLAG = false;                                               \
      return;                                                     \
    }                                                             \
  } while(0)

#endif // CMD_UTIL_H
