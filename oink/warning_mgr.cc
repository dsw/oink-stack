// see License.txt for copyright and terms of use

// quarl 2006-05-30

#include "warning_mgr.h"
#include "oink_util.h"

// TODO: make a command-line flag
const size_t WarningManager::maxWarnings = 2;

TailList<WarningManager> WarningManager::warningsToSummarize;

void WarningManager::emit(SourceLoc loc) {
  ++count;

  if (maxWarnings == 0 || count < maxWarnings) {
    userReportWarning(loc, "%s", text);
  } else if (count == maxWarnings) {
    userReportWarning(loc, "%s; further warnings will be suppressed", text);
  } else if (count == maxWarnings+1) {
    warningsToSummarize.append(this);
  } else if (count > maxWarnings+1) {
    // should already be on warningsToSummarize
  } else { xassert(0); }
}

// static
void WarningManager::summarize()
{
  if (warningsToSummarize.isEmpty()) return;

  userReportError(SL_UNKNOWN, "Omitted warnings:");

  FOREACH_TAILLIST(WarningManager, warningsToSummarize, iter) {
    WarningManager const *warning = iter.data();
    userReportError(SL_UNKNOWN, "   %s -- omitted %d times (total %d warnings)",
                    warning->text, warning->count - maxWarnings, warning->count);
  }
}
