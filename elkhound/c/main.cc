// main.cc            see license.txt for copyright and terms of use
// toplevel driver for the C parser

#include <iostream.h>     // cout
#include <stdlib.h>       // exit

#include "trace.h"        // traceAddSys
#include "parssppt.h"     // ParseTreeAndTokens, treeMain
#include "srcloc.h"       // SourceLocManager
#include "ckheap.h"       // malloc_stats
#include "c_env.h"        // Env
#include "c.ast.gen.h"    // C AST (r)
#include "strutil.h"      // plural
#include "cc_lang.h"      // CCLang
#include "treeout.h"      // treeOut
#include "parsetables.h"  // ParseTables
#include "c.gr.gen.h"     // CParse
#include "cyctimer.h"     // CycleTimer


// no bison-parser present, so need to define this
Lexer2Token const *yylval = NULL;


void if_malloc_stats()
{
  if (tracingSys("malloc_stats")) {
    malloc_stats();
  }
}


void doit(int argc, char **argv)
{
  traceAddSys("progress");
  //traceAddSys("parse-tree");

  // this is useful for emacs' outline mode, because if the file
  // doesn't begin with a heading, it collapses the starting messages
  // and doesn't like to show them again
  treeOut(1) << "beginning of output      -*- outline -*-\n";

  if_malloc_stats();

  SourceLocManager mgr;

  // string table for storing parse tree identifiers
  StringTable strTable;

  // parsing language options
  CCLang lang;
  lang.ANSI_Cplusplus();


  // --------------- parse --------------
  TranslationUnit *unit;
  {
    SemanticValue treeTop;
    ParseTreeAndTokens tree(lang, treeTop, strTable);

    CParse *user = new CParse(strTable, lang);
    tree.userAct = user;

    traceProgress() << "building parse tables from internal data\n";
    ParseTables *tables = user->makeTables();
    tree.tables = tables;

    CycleTimer timer;

    if (!treeMain(tree, argc, argv,
          "  additional flags for cparse:\n"
          "    malloc_stats       print malloc stats every so often\n"
          "    stopAfterParse     stop after parsing\n"
          "    printAST           print AST after parsing\n"
          "    stopAfterTCheck    stop after typechecking\n"
          "    printTypedAST      print AST with type info\n"
          "    tcheck             print typechecking info\n"
          "")) {
      // parse error
      exit(2);
    }

    traceProgress() << "done parsing (" << timer.elapsed() << ")\n";

    traceProgress(2) << "final parse result: " << treeTop << endl;
    unit = (TranslationUnit*)treeTop;

    //unit->debugPrint(cout, 0);

    delete user;
    delete tables;
  }

  checkHeap();

  // print abstract syntax tree
  if (tracingSys("printAST")) {
    unit->debugPrint(cout, 0);
  }

  if (tracingSys("stopAfterParse")) {
    return;
  }

                 
  // --------- declarations provided automatically -------
  Variable mem(HERE_SOURCELOC, strTable.add("mem"),
               new PointerType(PO_POINTER, CV_NONE,
                 &CVAtomicType::fixed[ST_INT]), DF_NONE);

  // ---------------- typecheck -----------------
  {
    traceProgress() << "type checking...\n";
    Env env(strTable, lang);
    env.addVariable(mem.name, &mem);
    unit->tcheck(env);
    traceProgress(2) << "done type checking\n";

    // print abstract syntax tree annotated with types
    if (tracingSys("printTypedAST")) {
      unit->debugPrint(cout, 0);
    }

    if (env.getErrors() != 0) {
      int n = env.getErrors();
      cout << "there " << plural(n, "was") << " " << env.getErrors() 
           << " typechecking " << plural(n, "error") << "\n";
      exit(4);
    }

    if (tracingSys("stopAfterTCheck")) {
      return;
    }
  }


  //malloc_stats();

  // delete the tree
  delete unit;
  strTable.clear();

  //checkHeap();
  //malloc_stats();

  traceRemoveAll();
}

int main(int argc, char **argv)
{
  doit(argc, argv);

  //malloc_stats();

  return 0;
}
