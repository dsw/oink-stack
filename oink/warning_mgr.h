// see License.txt for copyright and terms of use

// quarl 2006-05-30
//    WarningManager: a class for avoiding emitting the same warning
//    repeatedly.
//
//    usage: replace
//        userReportWarning(loc, "foo warning")
//    with:
//        static WarningManager wm("foo warning");
//        wm.emit(loc);
//
//    The 'static' is critical, since that is how we count warnings, and
//    it's added to a global list.
//
//    The warning text must be constant.  For now, it takes a const char *,
//    though this can be changed to a 'string' if necessary.

#ifndef WARNING_MGR_H
#define WARNING_MGR_H

#include "taillist.h"
#include "srcloc.h"

class WarningManager
{
  // how many times we have seen this warning (including omitted ones)
  size_t count;

  // text of the warning
  char const * const text;

public:
  WarningManager(char const *text0) : count(0), text(text0) {}

  // emit a single warning (unless we've seen it too many times)
  void emit(SourceLoc loc);

  // summarize any warnings we've omitted
  static void summarize();

protected:
  // maximum number of warnings before we start omitting
  static const size_t maxWarnings;

  // list of warnings which have been omitted
  static TailList<WarningManager> warningsToSummarize;
};


#endif
