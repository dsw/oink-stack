// see License.txt for copyright and terms of use

#include "cpdinit.h"

// forward
static void compoundInit0(void *context, SourceLoc loc, MemberValueIter &value_iter,
                          Variable *contentVar,
                          IN_compound *init,
                          oneAssignment_t *oneAssignment,
                          reportUserError_t *reportUserError);

static void oneInitializer(void *context, SourceLoc loc, MemberValueIter &value_iter,
                           Initializer *init,
                           oneAssignment_t *oneAssignment,
                           reportUserError_t *reportUserError) {
  try {
    if (init->isIN_compound() ||
        (init->isIN_expr() && init->asIN_expr()->getE()->skipGroups()->isE_compoundLit())
        ) {
      IN_compound *ic = NULL;
      if (init->isIN_compound()) ic = init->asIN_compound();
      else {
        xassert(init->isIN_expr());
        ic = init->asIN_expr()->getE()->skipGroups()->asE_compoundLit()->getInit();
      }
      xassert(ic);
      USER_ASSERT(!value_iter.done(), init->getLoc(),
                  "Too many initializers for type in compound initializer");
      MemberValueIter value_iter1(loc, &value_iter, value_iter.at());
      compoundInit0(context, loc, value_iter1,
                    value_iter.at(), ic, oneAssignment, reportUserError);
      value_iter.setSkipAcross(true); // go across the tree next time, not down
    } else if (IN_expr *ie = init->ifIN_expr()) {
      // deal with special exception where you can initialize a char array
      // using a string literal (or a cast to one, etc.)
      bool is_string_lit = MemberValueIter::isCharArray(ie->getE()->getAbstrValue());
      Variable *pointVar = value_iter.ensurePoint(is_string_lit);
      if (is_string_lit) {
        value_iter.setSkipAcross(true);
      }
      // quarl 2006-05-28: this indicates a real problem in the source code;
      // but gcc accepts with a warning (in C mode) so we should as well?
      USER_ASSERT(pointVar, init->getLoc(),
                  "Too many initializers for type at expression initializer"
                  " (ef9ea6b0-3391-4057-92b1-4d49f225dc0f)");
      oneAssignment(context, loc, value_iter, ie, value_iter.at_container(), pointVar);
    } else xfailure("illegal Initializer"); // IN_ctor shouldn't be able to get here
  }
  catch (UserError &e) {
    if (reportUserError(context, loc, value_iter, init, e)) throw; // rethrow
  }
}

static void compoundInit0(void *context, SourceLoc loc, MemberValueIter &value_iter,
                          Variable *contentVar,
                          IN_compound *init,
                          oneAssignment_t *oneAssignment,
                          reportUserError_t *reportUserError) {
  try {
    // this unfortunate non-orthogonality is due to the rule that you
    // can put extra curlies in as well
    if (MemberValueIter::isContainer(asVariable_O(contentVar)->abstrValue()->asRval())) {
      value_iter.adv();
    }
    bool firstTime = true;
    FOREACH_ASTLIST_NC(Initializer, init->getInits(), init_iter) {
      if (IN_designated *indesig = init_iter.data()->ifIN_designated()) {
        FakeList<Designator> *dl;
        dl = indesig->getDesignatorList();
        xassert(dl);
        // check for this kind of designator "[ 1 ... 3 ] =".  This is a gcc-ism:
        // http://gcc.gnu.org/onlinedocs/gcc-3.2.2/gcc/Designated-Inits.html#Designated%20Inits
        // This is ad-hoc and only supports a single range.  You can't
        // combine it with anything else like other designators in the
        // list or fields etc.
        SubscriptDesignator *sd = dl->first()->ifSubscriptDesignator();
        if (sd && sd->getIdx_expr2()) { // is a range designator
          USER_ASSERT(!dl->butFirst(), sd->getLoc(),
                      "range designators cannot have further trailing designators"
                      " in their designator list");
          USER_ASSERT(sd->getIdx_computed() <= sd->getIdx_computed2(), sd->getLoc(),
                      "start of range designator cannot be greater than end");
          for (int i=sd->getIdx_computed(); i <= /* yes <=, not < */ sd->getIdx_computed2(); ++i) {
            value_iter.reset(dl, &i);
            oneInitializer(context, loc, value_iter,
                           indesig->getInit(), oneAssignment, reportUserError);
          }
        } else {                  // is a normal subscript or field designator
          value_iter.reset(dl);
          oneInitializer(context, loc, value_iter,
                         indesig->getInit(), oneAssignment, reportUserError);
        }
      } else {
        if (!firstTime) value_iter.adv();
        oneInitializer(context, loc, value_iter,
                       init_iter.data(), oneAssignment, reportUserError);
      }
      firstTime = false;
    }
  }
  catch (UserError &e) {
    if (reportUserError(context, loc, value_iter, init->uptoInitializer(), e)) throw; // rethrow
  }
}

int compoundInit(void *context, SourceLoc loc, Variable *contentVar, IN_compound *init,
                 oneAssignment_t *oneAssignment,
                 reportUserError_t *reportUserError) {
  MemberValueIter value_iter1(loc, NULL, contentVar);
  compoundInit0(context, loc, value_iter1, contentVar, init, oneAssignment, reportUserError);
  return value_iter1.getMaxFirstFrameCursor();
}
