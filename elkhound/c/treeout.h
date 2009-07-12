// treeout.h            see license.txt for copyright and terms of use
// interface for printing trees; very small right now

// the idea is to print ascii marked in a way that Emacs' outline
// mode can read and display it as a tree

#ifndef TREEOUT_H
#define TREEOUT_H

#include <iostream.h>     // ostream

// print heading to stdout, with some indication of tree level
ostream &treeOut(int level);

#endif // TREEOUT_H
