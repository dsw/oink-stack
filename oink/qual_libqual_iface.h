// see License.txt for copyright and terms of use

// An interface to the libqual polymorphic qualifier backend.

// Namespace invariant: when something corresponds directly to
// something in qual, the name in this file is its name in qual
// suffixed with _CQUAL.  Sometimes things are named after the name of
// the pointer to the thing, for example in the case of types.

#ifndef LIBQUAL_IFACE_H
#define LIBQUAL_IFACE_H

#include "srcloc.h"

#include "qual_libqual_iface0.h"

namespace LibQual {
  extern "C" {
#include "libqual/qual_flags.h"
#include "libqual/qerror.h"
  }
};

// c/dtor the whole qualifier backend
void init_libqual(char *config_file);
void cleanup_libqual();

#if !defined DELAY_SET_NAMES
# error DELAY_SET_NAMES should be 0 or 1
#endif

// ctor qvars
LibQual::Type_qualifier *make_qvar_CQUAL(
#if !DELAY_SET_NAMES
  char const *name,
#endif
  SourceLoc loc,
  int preferred, int global);

// add edges between qvars
void mkinst_qual_CQUAL(SourceLoc loc,
                       LibQual::qual_gate *qgate,
                       LibQual::Type_qualifier *left, LibQual::Type_qualifier *right,
                       LibQual::polarity polarity,
                       int fcId,
                       int edgeNumber);
void mkleq_qual_CQUAL(SourceLoc loc,
                      LibQual::qual_gate *qgate,
                      LibQual::Type_qualifier *left, LibQual::Type_qualifier *right,
                      int edgeNumber);
void mkeq_qual_CQUAL(SourceLoc loc,
                     LibQual::qual_gate *qgate,
                     LibQual::Type_qualifier *left, LibQual::Type_qualifier *right,
                     int edgeNumber);
void unify_qual_CQUAL(SourceLoc loc,
                      LibQual::Type_qualifier *left, LibQual::Type_qualifier *right,
                      int edgeNumber);

// polarity
bool isPosOrNegPolarity(LibQual::polarity p);
LibQual::polarity invertPolarity(LibQual::polarity p);

#endif // LIBQUAL_IFACE_H
