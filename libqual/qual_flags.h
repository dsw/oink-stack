/* see License.txt for copyright and terms of use */

#ifndef QUAL_FLAGS_H
#define QUAL_FLAGS_H

#include "libqual/linkage.h"
#include "libqual/bool.h"

EXTERN_C_BEGIN

/* True if warning should be reported as errors. */
extern int report_warnings_as_errors;

/* True if we should print out the qualifier constraint graph in dot
   form. */
extern int flag_print_quals_graph;

/* True if we should display names in an ugly way so that you can tell
   different versions of the same name apart. */
extern int flag_ugly;

/* When this flag is TRUE, the type constraint solver is only required
   to compute the bounds on type variables with inconsistent bounds.
   This optimization can provide a big speedup. */
extern int flag_errors_only;

/* When this flag is true, print out an explanation for each of the
   errors, a la PAM mode.  */
extern int flag_explain_errors;

/* Don't filter errors */
extern int flag_all_errors;

/* Limit number of errors displayed; 0 for unlimited */
extern int flag_max_errors;

/* When this flag is TRUE, the type inference engine should perform a
   context-summary analysis in addition to the standard
   context-independent analysis. */
extern int flag_context_summary;

EXTERN_C_END

#endif
