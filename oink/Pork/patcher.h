#ifndef PATCHER_H
#define PATCHER_H
// See License.txt for copyright and terms of use

// Provides patch printing facilities

// dsw: The API for Patcher is less than clear to me, so after reading
// Taras's docs and having email conversations with him, here is my
// summary for people thinking of editing code as if you were editing
// a document:
//
//   copy    is getRange
//   insert  is insertBefore
//   replace is printPatch
//   delete  is printPatch("")
//
//   Not yet sure if cut works as (copy;delete).
//
// Note that the behavior when inserting twice at the same location
// seems to be not well defined; therefore one must maintain a mapping
// from locations to string to insert at that location and then insert
// them once.  In general attempt to avoid overlapping insertions.
// The mysterious "recursive" flag might help; see the conversation
// below with Taras.

// Taras Glek, Fri, Oct 23, 2009 at 4:06 PM:
//
//     Daniel Wilkerson wrote:
//     Can you give a hint as to what the recursive flag means?  I
//     don't see any recursion going on in your code.
//
// so if you have a function like
//
//   void foo(){
//     do_stuff();
//   }
//
// and you have 2 transformations.
//
//   1. Rename do_stuff to  mess_with_stuff()
//
//   2. Add function entry/exit markers
//
// So you want to end up with.
//
//   void foo() {
//     entry;
//     mess_with_stuff();
//     exit;
//   }
//
// Then you need to call printPatch bread-first(ie on do_stuff()) and
// then make sure you do printPatch on foo's body with the recursive
// flag.
//
// So recursion in this case means don't throw away mods nested in the
// chunk of code you are modifying.

// Taras Glek, Fri, Oct 23, 2009 at 5:05 PM:
//
//     Daniel Wilkerson wrote:
//     Shouldn't recurse just be true by default?  Why even allow it
//     to be false?
//
// Recursive rewrites make things a lot more complicated. I added it
// for a complex rewrite that we never landed. I believe it still has
// a few bugs, so I don't turn it on by default.

#include "cppundolog.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <srcloc.h>

// **** UnboxedLoc

// an unboxed source location
class UnboxedLoc {
public:
  unsigned int line;
  unsigned int col;

  UnboxedLoc(unsigned int line=0, unsigned int col=0)
    : line(line), col(col)
  {}

  // returns the filename since that isn't stored
  const char *set(SourceLoc loc);
  SourceLoc toSourceLoc(const char *file);
  int operator<(const UnboxedLoc &rhs) const {
    return line < rhs.line || (line == rhs.line && col < rhs.col);
  }
};

inline std::ostream& 
operator<<(std::ostream& os, const UnboxedLoc& pl) {
  return os << pl.line << ":" << pl.col;
}

// **** PairLoc

class PairLoc : public std::pair<CPPSourceLoc, CPPSourceLoc> {
public:
  PairLoc(CPPSourceLoc a, CPPSourceLoc b)
    : std::pair<CPPSourceLoc, CPPSourceLoc>(a, b)
  {}

  bool hasExactPosition() const {
    return first.hasExactPosition() && second.hasExactPosition();
  }

  MacroUndoEntry *getMacro() const {
    return first.macroExpansion
      ? first.macroExpansion
      : second.macroExpansion;
  }
};

// **** UnboxedPairLoc
 
class UnboxedPairLoc : public std::pair<UnboxedLoc, UnboxedLoc> {
public:
  UnboxedPairLoc(PairLoc const &pairLoc);
  UnboxedPairLoc(char const *, UnboxedLoc const &, UnboxedLoc const &);

  int operator<(const UnboxedPairLoc &rhs) const {
    int i = file.compare(rhs.file);
    return i < 0
      || (i == 0 
          && static_cast<std::pair<UnboxedLoc, UnboxedLoc> const &>(*this) 
          < static_cast<std::pair<UnboxedLoc, UnboxedLoc> const &>(rhs));
  }

  std::string toString() const;
  std::string file;
};

inline std::ostream& 
operator<<(std::ostream& os, const UnboxedPairLoc& pl) {
  return os << pl.file <<":("<< pl.first <<", "<< pl.second <<")";
}

// **** Patcher

typedef std::map<UnboxedPairLoc, std::string> patch_map;
typedef std::list<patch_map::value_type const *> hunk_queue;

class Patcher {
private:
  std::ostream& out;            // where this patch is going
  std::string file;
  std::vector<std::string> lines;
  patch_map output;
  std::string dir;
  bool recursive;

public:
  Patcher(std::ostream& out = std::cout, bool recursive = false);
  ~Patcher();
  void load(std::string const &file);

  // recursion allows bigger hunks to displace smaller ones
  void printPatch(std::string const &str, UnboxedPairLoc const &loc,
                  bool recursive = false);

  // returns line in file
  std::string getLine(unsigned int line, std::string const &file);

  // returns contents of file specified by mmlv
  std::string getRange(UnboxedPairLoc const &loc);

  unsigned int lineCount(std::string const &file);

  // sets the file from which the relative includes originate
  void setFile(std::string const &file);
  void flush();
  
  void insertBefore(char const *file, UnboxedLoc const &loc,
                    std::string const &str);
  void insertBefore(CPPSourceLoc const &csl, std::string const &str,
                    long offset = 0);
private:
  void printHunkHeaderAndDeletedLines
  (unsigned int minLine,
   unsigned int maxLine,
   int added_lines, std::string const &file,
   std::ostream &ostream);

  // copies the line range from file into a specified stream and adds a prefix
  void copy(unsigned int minLine, unsigned int maxLine,
            std::string const &file,
            std::ostream &ostream, std::string const &prefix);

  std::string resolveAbsolutePath(std::string const &path) const;
  //flushes a hunk at a time
  void flushQueue(hunk_queue &q);
};

#endif // PATCHER_H
