// see License.txt for copyright and terms of use

// All globals for the qual analysis in one place.

#ifndef QUAL_GLOBAL_H
#define QUAL_GLOBAL_H

#include "oink_global.h"
#include "qual_value_print.h"
#include "qual_cmd.h"

// An aliased global reference to oinkCmd, but downcast ahead of time for
// convenience.  I didn't want to have to create this globally, but
// there is no other way to get it into a few places like the ctor of
// Var_Q;
extern QualCmd *qualCmd;

// true iff we have run finish_quals(); There are lots of things you
// must not do after you have run finish_quals and this allows for
// lots of assertions at those points.
extern bool haveRunFinishQuals;

// a source for unique id-s for inference edges
extern int globalEdgeNumber;

#endif // QUAL_GLOBAL_H
