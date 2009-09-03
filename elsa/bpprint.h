// bpprint.h
// C++ AST pretty printer based on smbase boxprint module.

// Basically a re-do of cc_print.{h,cc,ast}, but this time exclusively
// using the syntactic structure and ignoring the type (etc.)
// annotations.  Plan is to use it for cc2c.

#ifndef BPPRINT_H
#define BPPRINT_H

// smbase
#include "boxprint.h"        // BoxPrint

// libc++
#include <iostream>          // cout

class TranslationUnit;       // cc_ast.h


// wrapper around BoxPrint to carry additional state specific to
// printing the AST
class BPEnv : public BoxPrint {
};


// Pretty print an entire translation unit and send it to 'out'.
void bppTranslationUnit(std::ostream &out, TranslationUnit const &unit);


#endif // BPPRINT_H
