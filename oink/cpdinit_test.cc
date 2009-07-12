// see License.txt for copyright and terms of use

#include "cpdinit_test.h"       // this module
#include "cpdinit_test_visitor.h"
#include "oink_global.h"        // oinkCmd
#include "oink.gr.gen.h"        // CCParse_Oink

// needed so the walk can non-locally communicate the name of the
// variable being initialized to the callback code that needs to print
// it
char const *cpdTestGlobalCurVarName;

void CpdInitTest::compoundTest_stage() {
  Restorer<bool> restorer(value2typeIsOn, true);
  foreachSourceFile {
    File *file = files.data();
    maybeSetInputLangFromSuffix(file);
    TranslationUnit *unit = file2unit.get(file);
    CpdInitTestVisitor v(cout);
    unit->traverse(v.loweredVisitor);
  }
}
