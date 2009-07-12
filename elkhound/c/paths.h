// paths.h            see license.txt for copyright and terms of use
// AST routines for enumerating paths

// unfortunately it turns out that there is a /usr/include/paths.h
// on some systems, so inclusion order is important..

#ifndef PATHS_H
#define PATHS_H

#include "c.ast.gen.h"      // C AST elements
#include "sobjlist.h"       // SObjList

class Env;                  // cc_env.h

// instrument the AST of a function to enable printing (among other
// things) of paths; returns total # of paths in the function (in
// addition to storing that info in the AST)
int countPaths(Env &env, TF_func *func);

// print all paths in this function
void printPaths(TF_func const *func);
void printPathFrom(SObjList<Statement /*const*/> &path, int index,
                   Statement const *node, bool isContinue);

// count/print for statements
int countExprPaths(Statement const *stmt, bool isContinue);
void printExprPath(int index, Statement const *stmt, bool isContinue);

// count/print for inititializers
int countExprPaths(Initializer const *init);
void printExprPath(int index, Initializer const *init);

// count/print for expressions
int countPaths(Env &env, Expression *ths);
void printPath(int index, Expression const *ths);

// starting from a point above 'stmt', how many paths go into 'stmt'
// (and possibly beyond)?
int numPathsThrough(Statement const *stmt);

#endif // PATHS_H
