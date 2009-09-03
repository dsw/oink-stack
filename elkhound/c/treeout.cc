// treeout.cc            see license.txt for copyright and terms of use
// code for treeout.h

#include "treeout.h"     // this module

// print heading to stdout, with some indication of tree level
std::ostream &treeOut(int level)
{
  while (level--) {
    std::cout << "*";
  }
  std::cout << " ";
  return std::cout;
}
