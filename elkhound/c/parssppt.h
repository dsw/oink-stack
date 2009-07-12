// parssppt.h            see license.txt for copyright and terms of use
// parser-support routines, for use at runtime while processing
// the generated Parse tree
//
// this module is primarily for use with the C and C++ grammars,
// but has been pressed into service for a few other uses too;
// new grammars and parsers should probably not use this

#ifndef __PARSSPPT_H
#define __PARSSPPT_H

#include "lexer2.h"       // Lexer2
#include "useract.h"      // SemanticValue, UserAction

class ParseTables;


// ----------------- helpers for analysis drivers ---------------
// a self-contained parse tree (or parse DAG, as the case may be)
class ParseTreeAndTokens {
public:
  // reference to place to store final semantic value
  SemanticValue &treeTop;

  // we need a place to put the ground tokens
  Lexer2 lexer2;

  // parse parameter
  UserActions *userAct;            // (serf)

  // parse tables (or NULL)
  ParseTables *tables;             // (serf)

public:
  ParseTreeAndTokens(CCLang &lang, SemanticValue &top);
  ParseTreeAndTokens(CCLang &lang, SemanticValue &top, StringTable &extTable);
  ~ParseTreeAndTokens();
};


// dsw: what is this?
//  // given grammar and input, yield a parse tree
//  // returns false on error
//  bool toplevelParse(ParseTreeAndTokens &ptree, char const *grammarFname,
//                     char const *inputFname, char const *symOfInterestName);

bool toplevelParse(ParseTreeAndTokens &ptree, char const *inputFname);

char *processArgs(int argc, char **argv, char const *additionalInfo = NULL);

void maybeUseTrivialActions(ParseTreeAndTokens &ptree);

// useful for simple treewalkers; false on error
bool treeMain(ParseTreeAndTokens &ptree, int argc, char **argv,
              char const *additionalInfo = NULL);


#endif // __PARSSPPT_H
