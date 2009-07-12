// see License.txt for copyright and terms of use

// Process one Value/compound-initializer pair, generating dataflow
// edges.

#ifndef CPDINIT_H
#define CPDINIT_H

// During preprocessing, set the variable CPDINIT_LIB_HEADERS to
// point to your replacement for cpdinit_lib.h.  See the
// "building" section of the Readme file for more on this.
#include CPDINIT_LIB_HEADERS

#include "member_type_iter.h"

// This function is called to notify you about a discovered pair,
// assigning the AST node for an expression initializer src to the
// type node tgt.  The loc is just of the whole compound initializer
// expression, so not very useful but I need it.
typedef void oneAssignment_t
  (void *context, SourceLoc loc, MemberValueIter &value_iter, IN_expr *src_expr,
   Value *tgtContainer, Variable *tgtContentVar);

// You must implement a function to handle user errors; Return true if
// you want the exception rethrown.  Again, the loc isn't very
// precise, but the init is.
typedef bool reportUserError_t
  (void *context, SourceLoc loc, MemberValueIter &value_iter, Initializer *init, UserError &e);

// returns the max index of the top object (-1 if never set), so that
// when initializing a size-unspecified array with a compound
// initializer, you can find how big to set the array size to by
// adding one to the return value
int compoundInit(void *context, SourceLoc loc, Variable *contentVar, IN_compound *init,
                 oneAssignment_t *oneAssignment,
                 reportUserError_t *reportUserError);

#endif // CPDINIT_H
