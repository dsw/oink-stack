#ifndef PIGLET_H
#define PIGLET_H

#include "parssppt.h"          // ParseTreeAndTokens, treeMain
#include "srcloc.h"            // SourceLocManager
#include "cc_env.h"            // Env
#include "cc_ast.h"            // C++ AST (r)
#include "cc_lang.h"           // CCLang
#include "parsetables.h"       // ParseTables
// #include "cc.gr.gen.h"         // CCParse
#include "oink.gr.gen.h"        // CCParse_Oink
#include "cc_elaborate.h"       // ElabVisitor
#include "cc_ast_aux.h"         // class LoweredASTVisitor

// not being used but you should probably be using them
// #include "ckheap.h"             // malloc_stats
// #include "integrity.h"          // IntegrityVisitor

// you might want these
// #include "nonport.h"            // getMilliseconds
// #include "cc_print.h"           // PrintEnv

#include <stdlib.h>             // exit, getenv, abort
#include <functional>

// C++ Parser
//
// Note: the pointer returned by getAST is valid only as long as this
//       object is still alive. (It secretly relies on the private data
//       manager fields declared here.)
class PigletParser {
public:
  PigletParser();

  // Parse the file and call the given fn with the AST.
  template<typename R>
  static R apply(std::unary_function<TranslationUnit*,R> func,
                 const char *filename);

  // Parse the file and return the AST. This may throw Elsa exns.
  TranslationUnit *getAST(const char *filename);
  // Parse the file and return the AST. Die on errors.
  TranslationUnit *getASTNoExc(const char *filename);

private:
  CCLang lang;
  
  SourceLocManager mgr;
  StringTable strTable;
  BasicTypeFactory tfac;
  
  ArrayStack<Variable*> madeUpVariables;
  ArrayStack<Variable*> builtinVars;

  int parseWarnings;

  TranslationUnit *parse(const char *inputFname);
  void typeCheck(TranslationUnit *unit);
  void elaborate(TranslationUnit *unit);
};

#endif // PIGLET_H
