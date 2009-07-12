/* see License.txt for copyright and terms of use */

#include "libqual/qual_flags.h"

/* True if warning should be reported as errors. */
int report_warnings_as_errors = 0;

/* Set (externally) to true if we should print out the qualifier
   constraint graph in dot form. */
int flag_print_quals_graph = 0;

/* True if we should display names in an ugly way so that you can tell
   different versions of the same name apart. */
int flag_ugly = 1;              // dsw: I changed this to default to 1

/* When this flag is TRUE, the type inference engine is only required
   to compute the bounds on type variables with inconsistent bounds.
   This optimization can provide a big speedup. */
int flag_errors_only = 0;

/* When this flag is true, print out an explanation for each of the
   errors, a la PAM mode.  */
int flag_explain_errors = 1;

/* Don't filter errors */
int flag_all_errors = 0;

/* Limit number of errors displayed; 0 for unlimited */
int flag_max_errors = 0;

/* When this flag is TRUE, the type inference engine should perform a
   context-summary analysis in addition to the standard
   context-independent analysis. */
int flag_context_summary = 0;
