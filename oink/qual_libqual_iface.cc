// see License.txt for copyright and terms of use.

// interface to libqual

#include "xassert.h"
#include <stdlib.h>
#include <stdio.h>

#include "libregion/regions.h"

// NOTE: due to the strange interaction of extern "C" down within
// namespaces, this has to go *after* the inclusion of the standard
// header files above
#include "qual_libqual_iface.h" // this module

#if defined USE_SMBASE_SRCLOC
static inline LibQual::location new_Location(SourceLoc loc)
{
  return loc;
  // return LibQual::make_location((int) loc, index);
}
#else
static inline LibQual::location new_Location(SourceLoc loc)
{
  struct LibQual::Location *temp_loc = (struct LibQual::Location *) malloc(sizeof *temp_loc);
  temp_loc->filename = const_cast<char*> (sourceLocManager->getFile(loc));
  temp_loc->lineno = sourceLocManager->getLine(loc);
  temp_loc->filepos = sourceLocManager->getOffset(loc);
  // temp_loc->location_index = index;
  xassert(temp_loc->filename);
  return temp_loc;
}
#endif

// c/dtor the whole qualifier backend
void init_libqual(char *config_file) {
  region_init();
  LibQual::init_s18n();
  LibQual::s18n_register_type(&LibQual::qual_set_type);
  LibQual::init_quals();
  LibQual::init_qerror();
  LibQual::init_location();

  if (config_file) LibQual::load_config_file_quals(config_file);

  // Add const so that we can do -fconst-subtyping no matter what
  if (!LibQual::const_qual) {
    LibQual::begin_po_qual();
    LibQual::const_qual = LibQual::add_qual("const");
    LibQual::add_level_qual(LibQual::const_qual, LibQual::level_ref);
    LibQual::set_po_nonprop();
    LibQual::end_po_qual();
  }

  // Add volatile so we can handle noreturn functions
  if (!LibQual::volatile_qual) {
    LibQual::begin_po_qual();
    LibQual::volatile_qual = LibQual::add_qual("volatile");
    LibQual::add_level_qual(LibQual::volatile_qual, LibQual::level_ref);
    LibQual::add_sign_qual(LibQual::volatile_qual, LibQual::sign_eq);
    LibQual::set_po_nonprop();
    LibQual::end_po_qual();
  }

  LibQual::end_define_pos();    // Allow cqual to run with no qualifiers
}

void cleanup_libqual() {
  LibQual::reset_libqual();

  // this will deallocate the 'permanent' region, making sure everything has
  // been deallocated so far.
  // region_cleanup();
}


// ctor qvars
LibQual::Type_qualifier *make_qvar_CQUAL(
#if !DELAY_SET_NAMES
  char const *name,
#endif
  SourceLoc loc,
  int preferred, int global)
{
  LibQual::Type_qualifier *tmp_qvar;
  // xassert(name);
  tmp_qvar = (LibQual::Type_qualifier*)
    LibQual::make_qvar(
#if !DELAY_SET_NAMES
      name,
#endif
      new_Location(loc),
      -1,
      preferred,
      global);
  xassert(tmp_qvar);
  return tmp_qvar;
}

#if 0

// allow edge insertion to be intercepted easily in the debugger
static void ackackack2(int*x) {}// from smbase/breaker.cpp

static void make_edge_before() {
  static int i=0;               // all this junk is just to make sure
  int a=1;                      // that this function has a complete
  ackackack2(&a);               // stack frame, so the debugger can unwind
  i++;                          // the stack
}

static void make_edge_after() {
  static int i=0;               // all this junk is just to make sure
  int a=1;                      // that this function has a complete
  ackackack2(&a);               // stack frame, so the debugger can unwind
  i++;                          // the stack
}

#else
#define make_edge_before() ((void)0)
#define make_edge_after() ((void)0)
#endif

// add edges between qvars
void mkinst_qual_CQUAL(SourceLoc loc,
                       LibQual::qual_gate *qgate,
                       LibQual::Type_qualifier *left, LibQual::Type_qualifier *right,
                       LibQual::polarity polarity,
                       int func_call_id,
                       int edgeNumber
                       ) {
  make_edge_before();

  xassert(polarity!=LibQual::p_non);
  xassert(func_call_id!=-1);
  xassert(left);
  xassert(right);
  LibQual::mkinst_qual_en(new_Location(loc),
                          func_call_id,
                          qgate,
                          left, right,
                          polarity,
                          NULL, edgeNumber);
  make_edge_after();
}

void mkleq_qual_CQUAL(SourceLoc loc,
                      LibQual::qual_gate *qgate,
                      LibQual::Type_qualifier *left, LibQual::Type_qualifier *right,
                      int edgeNumber
                      ) {
  make_edge_before();
  xassert(left);
  xassert(right);
  mkleq_qual_en(new_Location(loc),
                -1,
                qgate,
                left,
                right,
                NULL,
                edgeNumber);
  make_edge_after();
}

void mkeq_qual_CQUAL(SourceLoc loc,
                     LibQual::qual_gate *qgate,
                     LibQual::Type_qualifier *left, LibQual::Type_qualifier *right,
                     int edgeNumber  // unique id for the edge(s)
                     ) {
  make_edge_before();
  xassert(left);
  xassert(right);
  mkeq_qual_en(new_Location(loc),
               -1,
               qgate,
               left,
               right,
               NULL,
               edgeNumber);
  make_edge_after();
}

void unify_qual_CQUAL(SourceLoc loc,
                      LibQual::Type_qualifier *left, LibQual::Type_qualifier *right,
                      int edgeNumber
                      ) {
  make_edge_before();
  xassert(left);
  xassert(right);
  unify_qual_en(new_Location(loc),
                -1,
                left,
                right,
                NULL,
                edgeNumber);
  make_edge_after();
}

// polarity
bool isPosOrNegPolarity(LibQual::polarity p) {
  switch(p) {
  default: xfailure("illegal polarity");
  case LibQual::p_neg: return true;
  case LibQual::p_non: return false; // I don't ever use this, though it is well-defined
  case LibQual::p_pos: return true;
  case LibQual::p_sub: return false; // I don't ever use this, though it is well-defined
  }
}

LibQual::polarity invertPolarity(LibQual::polarity p) {
  switch(p) {
  default: xfailure("illegal polarity");
  case LibQual::p_neg: return LibQual::p_pos;
  case LibQual::p_non: return LibQual::p_non;
  case LibQual::p_pos: return LibQual::p_neg;
  case LibQual::p_sub: xfailure("illegal polarity"); // FIX: what do I do here?
  }
}
