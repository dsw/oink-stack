/* see License.txt for copyright and terms of use */

#include "libqual/quals.h"
#include "libqual/qerror.h"

/* Call this at any time to reclaim all memory used by libqual and to
   reset libqual to its initial state (i.e., the state when the
   library has just been loaded).  Note: after calling this function,
   you will again need to call init_quals etc. */
void reset_libqual(void) {
  reset_quals();
  reset_location();
  reset_qerror();
  reset_location();
  reset_s18n();
}
