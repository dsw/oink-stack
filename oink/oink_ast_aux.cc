// see License.txt for copyright and terms of use

// Implementation of functions declared in the oink.ast file.

#include "cc_ast.h"             // C++ AST
#include "cc_type.h"            // typesystem
#include "oink_var.h"           // asVariable_O
#include "oink_global.h"        // astPrinter
#include "oink_util.h"

// Copied verbatim from cc_ast_aux.cc: ****************

// set the 'next' field of 'main' to 'newNext', with care given
// to possible ambiguities
template <class NODE>
void genericSetNext(NODE *main, NODE *newNext)
{
  // 'main's 'next' should be NULL; if it's not, then it's already
  // on a list, and setting 'next' now will lose information
  xassert(main->next == NULL);
  main->next = newNext;

  // FIX: removed since has no member ambiguity
  // if 'main' has any ambiguous alternatives, set all their 'next'
  // pointers too
//    if (main->ambiguity) {
//      genericSetNext(main->ambiguity, newNext);     // recursively set them all
//    }
}

// end verbatim copy ****************

// getBaseLoc ****************

SourceLoc D_name::getBaseLoc() {return loc;}
SourceLoc D_pointer::getBaseLoc() {return base->getBaseLoc();}
SourceLoc D_reference::getBaseLoc() {return base->getBaseLoc();}
SourceLoc D_func::getBaseLoc() {return base->getBaseLoc();}
SourceLoc D_array::getBaseLoc() {return base->getBaseLoc();}
SourceLoc D_bitfield::getBaseLoc() {return loc;}
SourceLoc D_ptrToMember::getBaseLoc() {return base->getBaseLoc();}
SourceLoc D_grouping::getBaseLoc() {return base->getBaseLoc();}

// assorted accessors ****************

// Scott seems to have implemented this now in elsa
//  Type *Expression::getType() {return type;}

SourceLoc Initializer::getLoc() {return loc;}
Expression *IN_expr::getE() {return e;}
ASTList<Initializer> &IN_compound::getInits() {return inits;}
FakeList<Designator> *IN_designated::getDesignatorList() {return designator_list;}
Initializer *IN_designated::getInit() {return init;}

SourceLoc Designator::getLoc() {return loc;}
StringRef FieldDesignator::getId() {return id;}
Expression *SubscriptDesignator::getIdx_expr() {return idx_expr;}
Expression *SubscriptDesignator::getIdx_expr2() {return idx_expr2;}
int SubscriptDesignator::getIdx_computed() {return idx_computed;}
int SubscriptDesignator::getIdx_computed2() {return idx_computed2;}

// Semantically, an explicit _up_cast.  This prevents me from
// explicitly specifying the inheritance heirarchy among the three
// Initializer classes in case the other clients of LibCpdInit want to
// do their hierarchy differently.
Initializer *IN_compound::uptoInitializer() {return this;}

IN_compound *E_compoundLit::getInit() {return init;}

// toString ****

void TypeSpecifier::printExtras_O(ostream &os, int indent) const {
  astPrinter->TypeSpecifier_printExtras(this, os, indent);
}


// icfg ****

// control flow graph implementation for ast nodes that were added by
// qual.ast; the implementation turns out to be to do nothing, but
// that is a coincidence

void S_change_type::icfg(CFGEnv &env)
{
  // this CFG does not model apparent flow of control present in
  // things like short-circuit evaluation of boolean expressions
}

void S_assert_type::icfg(CFGEnv &env)
{
  // this CFG does not model apparent flow of control present in
  // things like short-circuit evaluation of boolean expressions
}
