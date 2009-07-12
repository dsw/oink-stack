// see License.txt for copyright and terms of use

#include "oink_util.h"

#include <cstdio>
#include <cstring>
#include <cstdarg>

char const *argv0 = NULL;

inline const char *oink_basename(const char *pathname)
{
  const char *p = std::strrchr(pathname, '/');
  return p ? p+1 : pathname;
}

void set_argv0(const char *argv00)
{
  argv0 = oink_basename(argv00);
}

enum UserReportCommonType {
  REPORT_WARNING,
  REPORT_ERROR,
  REPORT_INFERENCE_ERROR,
  REPORT_FATAL_ERROR
};

inline void userReportCommon(SourceLoc loc, UserReportCommonType type,
                             const char *msg, va_list arg)
{
  FILE *out = stderr;

  if (argv0) {
    std::fprintf(out, "%s: ", argv0);
  }

  if (loc != SL_UNKNOWN) {
    std::fprintf(out, "%s:%d: ",
                 sourceLocManager->getFile(loc),
                 sourceLocManager->getLine(loc));
  }

  if (type == REPORT_WARNING) {
    std::fprintf(out, "warning: ");
  }

  std::vfprintf(out, msg, arg);

  std::fprintf(out, "\n");
}

void userReportWarning(SourceLoc loc, const char *msg, ...)
{
  va_list arg;
  va_start(arg, msg);
  userReportCommon(loc, REPORT_WARNING, msg, arg);
  va_end(arg);
}

void userReportError(SourceLoc loc, const char *msg, ...)
{
  va_list arg;
  va_start(arg, msg);
  userReportCommon(loc, REPORT_ERROR, msg, arg);
  va_end(arg);
}

void userInferenceError(SourceLoc loc, const char *msg, ...)
{
  va_list arg;
  va_start(arg, msg);
  userReportCommon(loc, REPORT_INFERENCE_ERROR, msg, arg);
  va_end(arg);

  breaker();
  throw UserError(INFERENCE_FAILURE_ExitCode);
}

void userFatalError(SourceLoc loc, const char *msg, ...)
{
  va_list arg;
  va_start(arg, msg);
  userReportCommon(loc, REPORT_FATAL_ERROR, msg, arg);
  va_end(arg);

  breaker();
  throw UserError(USER_ERROR_ExitCode);
}
