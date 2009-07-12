// see License.txt for copyright and terms of use

// Some basic utilities for reporting user errors with line numbers
// and other standard functionality.

#ifndef OINK_UTIL_H
#define OINK_UTIL_H

#include <iostream>
#include "exc.h"
#include "srcloc.h"

enum ExitCode {
  NORMAL_ExitCode             = 0,
  USER_ERROR_ExitCode         = 1,
  LEX_OR_PARSE_ERROR_ExitCode = 2,
  TYPECHECKING_ERROR_ExitCode = 4,
  LINK_ERROR_ExitCode         = 8,
  INFERENCE_FAILURE_ExitCode  = 32,
  INTERNALERROR_ExitCode      = 64,
};

class UserError : public xBase {
  public:
  ExitCode exitCode;
  UserError(ExitCode exitCode0, char const *message0 = "")
    : xBase(message0)
    , exitCode(exitCode0)
  {}
};

void userReportWarning(SourceLoc loc, const char *msg, ...);
void userReportError(SourceLoc loc, const char *msg, ...);
void userInferenceError(SourceLoc loc, const char *msg, ...) NORETURN;
void userFatalError(SourceLoc loc, const char *msg, ...) NORETURN;

#define USER_ASSERT(PRED, LOC, ARGS...)                           \
do {                                                              \
  if (!(PRED)) {                                                  \
    userFatalError(LOC, ARGS);                                    \
  }                                                               \
} while(0)

void cat_proc_status(std::ostream &out);

void explain_bad_alloc();
std::ostream &explain_process_stats(std::ostream &o);

extern char const *argv0;
void set_argv0(const char *argv00);

#endif // OINK_UTIL_H
