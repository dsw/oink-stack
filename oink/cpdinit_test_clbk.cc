// see License.txt for copyright and terms of use

#include "cpdinit_test_clbk.h"  // this module
#include "cc_print.h"
#include "value_print.h"
#include "oink_global.h"        // TypePrinterOink typePrinterOink
#include <iostream>             // cout

extern char const *cpdTestGlobalCurVarName; // cpdinit_test.cc

void oneAssignmentCpdTest(void *context, SourceLoc loc, MemberValueIter &type_iter,
                          IN_expr *src_expr, Value *tgtContainer, Variable *tgtContentVar) {
  // FIX: replace this with tgtContentVar->name
  std::cout << cpdTestGlobalCurVarName;
  std::cout << type_iter.toString();
  std::cout << " = ";
  OStreamOutStream out0(std::cout);
  CodeOutStream codeOut(out0);
  PrintEnv e(typePrinterOink, &codeOut);
  src_expr->e->iprint(e);
  codeOut << ";" << std::endl;
  codeOut.finish();
}

bool reportUserErrorCpdTest(void *context, SourceLoc loc, MemberValueIter &type_iter,
                            Initializer *init, UserError &e) {
  std::cout << "**** User error at " << ::toString(loc) << std::endl;
  return true;                  // rethrow
}

// must provide definition of MemberValueIter::toString since we
// actually use it

char const *MemberValueIter::toString() {
  stringBuilder s;
  if (parentIter) s << parentIter->toString();
  // We can't use a list iter like this:
  //   SFOREACH_OBJLIST(Frame, stack0.list, iter)
  // because we iterate in reverse.

  // NOTE: the "> 0" is to skip the last frame
  for(int i=stack0.list.count()-1; i>0; --i) {
    Frame *f = stack0.list.nth(i);
//      if (f->cursor == -1) {
//        xassert(i==0);
//        continue;
//      }
    xassert(f->cursor >= 0);
    if (asVariable_O(f->content)->abstrValue()->asRval()->t()->isArrayType()) {
      s << "[" << f->cursor << "]";
    } else if (CompoundType *ct =
               asVariable_O(f->content)->abstrValue()->asRval()->t()->ifCompoundType()) {
      char const *name = ct->getDataVariablesInOrder().nth(f->cursor)->name;
      xassert(name);
      s << "." << name;
    } else {
      xassert(i == 0);          // must be a leaf
    }
  }
  return strdup(s);             // FIX: garbage
};
