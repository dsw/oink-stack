// see License.txt for copyright and terms of use

// $Id: dbg_util.cc 1908 2006-05-14 21:58:54Z quarl $
#include "oink_util.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

#include <unistd.h>                                 // sbrk
#include <sys/resource.h>                           // getrlimit

#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

inline void cat(std::ostream &out, const char* filename, const char* prefix)
{
  // use syscalls instead of iostreams (or even stdio) because we have no
  // memory to allocate :(
  int f = open(filename, O_RDONLY);
  if (f == -1) return;
  // std::ifstream f(filename);
  // if (!f) return;
  char c;
  bool newline = true;
  // while (f.get(c)) {
  while (1==read(f,&c,1)) {
    if (newline) {
      out << prefix;
    }
    out << c;
    newline = (c == '\n');
  }
  close(f);
}

void cat_proc_status(std::ostream &out)
{
  out << argv0 << ":     /proc/self/status:\n";
  cat(out, "/proc/self/status", "                 ");
}
#else
void cat_proc_status(std::ostream &out) {
}
#endif

#include <ctime>
static inline unsigned long get_cpu_milliseconds() {
  clock_t c = clock();
  return c * 1000 / CLOCKS_PER_SEC;
}

// #ifdef __linux__
// #include <sys/times.h>
// static inline unsigned long get_cpu_milliseconds()
// {
//   struct tms tms[1];
//   times(tms);
//   return tms->tms_utime * 1000 / CLOCKS_PER_SEC;
// }
// #else
// #define get_cpu_milliseconds getMilliseconds // smbase/nonport.h
// #endif

// #include "nonport.h"            // (smbase) getMilliseconds
static inline
unsigned long process_time_msecs()
{
  static long msecs = get_cpu_milliseconds();
  return get_cpu_milliseconds() - msecs;
}

#ifdef __linux__
static inline
unsigned long process_mem_usage_mb()
{
  FILE *file = fopen ("/proc/self/stat", "r");
  if (!file)
    return 0;

  unsigned long vsize;
  // Note: some of these scanf specifiers would normally be 'long' versions if
  // not for the fact that we are using suppression (gcc warns).  see 'man
  // proc' for scanf specifiers and meanings.
  fscanf (file,
          "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u "
          "%*u %*u %*d %*d %*d %*d %*d %*d %*d "
          "%lu",                // vsize
          &vsize);
  fclose (file);

  return vsize / 1024 / 1024;
}
#else

static inline
unsigned long process_mem_usage_mb()
{
  // poor man's version; ignores mmap, etc.
  return (int)sbrk(0) / (1024 *1024);
}
#endif


std::ostream &explain_process_stats(std::ostream &o)
{
  o << " [" << std::setw(5) << process_time_msecs() << " ms]"
    << " [" << std::setw(4) << process_mem_usage_mb() << " MB] ";
  return o;
}

using std::cerr;
void explain_bad_alloc()
{
  cerr << argv0 << ": out of memory\n";

  // report current brk
  void *cur_brk = sbrk(0);
  cerr << argv0 << ":     brk = " << cur_brk << '\n';

  // report current rlimit
  rlimit rlim[1];
  getrlimit(RLIMIT_AS, rlim);
  cerr << argv0 << ":     RLIMIT_AS: soft = " << rlim->rlim_cur
       << ", hard = " << rlim->rlim_max << '\n';

  // report current rusage
  rusage usage[1];
  getrusage(RUSAGE_SELF, usage);
  cerr << argv0 << ":     RUSAGE_SELF: maxrss = " << usage->ru_maxrss
       << ", ixrss = " << usage->ru_ixrss
       << ", idrss = " << usage->ru_idrss
       << ", isrss = " << usage->ru_isrss
       << '\n';

  cat_proc_status(cerr);

  // exit(INTERNALERROR_ExitCode);
  abort();
}
