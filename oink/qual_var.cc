// see License.txt for copyright and terms of use

// These functions would go on a "qual annot variable" class except
// that there is no extra state so no such extra class is necessary.

#include "qual_var.h"            // this module
#include "qual.h"                // linker
#include "qual_annot.h"          // QualAnnot
#include "qual_value_children.h" // treeContainsQualAnnots
#include "qual_global.h"         // qualCmd
#include "cc_type.h"             // Type

Variable_Q::Variable_Q(XmlReader &xmlReader)
  : Variable_O(xmlReader)
{}

Variable_Q::Variable_Q(SourceLoc L, StringRef n, Type *t, DeclFlags f)
  : Variable_O(L, n, t, f)
{}

// attach appropriate name to all the values in this variable's value
// tree
void Variable_Q::nameTree() {
#if !DELAY_SET_NAMES
  xassert(qualCmd->name_vars);
#endif

  // sm: The following code segfaults when the Variable is
  // the one I create for the global namespace.  How is it that
  // this hasn't been a problem before, for other namespaces?
  if (hasFlag(DF_NAMESPACE)) return;

  // skip templates
  if (!getReal()) return;

  Value *value0 = abstrValue();

  if (!value0->isReferenceValue()) {
    value0 = value0->asLval();
  }
  // NOTE: it is generally bad for a value to have a NULL name.

  if (name) qa(value0)->nameTree(name);
  else qa(value0)->nameTree(stringc << "anon_var " << abstrValue()->t()->toMLString());
}

// attach appropriate globalness to all the values in this variable's
// value tree
void Variable_Q::setGlobalTree() {
  // sm: same as above...
  if (hasFlag(DF_NAMESPACE)) return;

  // skip templates
  if (!getReal()) return;

  xassert(!filteredOut());

  Globalness globalness = GLOBAL_Globalness; // err on the side of being global
  // NOTE: DF_AUTO just means the auto keyword was used, which it
  // usually isn't, so it is no good to check it
  bool isAuto = !hasFlag(DF_GLOBAL) && !hasFlag(DF_MEMBER);
  bool isStatic = hasFlag(DF_STATIC);
  // Rob says that global equals mutable and can escape a stack frame
  if ( (isAuto && !isStatic)  ||
       hasFlag(DF_ENUMERATOR) ||
       hasFlag(DF_TYPEDEF)    ) {
    globalness = LOCAL_Globalness;
  }
  qa(abstrValue())->setGlobalTree(globalness);
}

bool Variable_Q::treeContainsQvars() {
  // don't create an abstract value if there isn't one
  if (!hasAbstrValue()) return false;
  return treeContainsQvars_Value_Q(abstrValue());
}
