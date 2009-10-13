#ifndef PRCHECK_PARSER
#define PRCHECK_PARSER

// See License.txt for copyright and terms of use

// Provides patch printing facilities

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <srcloc.h>
#include "cppundolog.h"

class UnboxedLoc {
public:
  UnboxedLoc(unsigned int line = 0, unsigned int col = 0) : line(line), col(col) {
  }

  // returns the filename since that isn't stored
  const char* set(SourceLoc loc);
  SourceLoc toSourceLoc(const char* file);
  int operator<(const UnboxedLoc &rhs) const {
    return line < rhs.line
      || (line == rhs.line && col < rhs.col);
  }
  
  unsigned int line;
  unsigned int col;
};

inline std::ostream& 
operator<<(std::ostream& os, const UnboxedLoc& pl) {
    return os << pl.line << ":" << pl.col;
}

class PairLoc : public std::pair<CPPSourceLoc, CPPSourceLoc> {
public:
  PairLoc(CPPSourceLoc a, CPPSourceLoc b) : 
    std::pair<CPPSourceLoc, CPPSourceLoc>(a, b) {
  }

  bool hasExactPosition() const {
    return first.hasExactPosition() && second.hasExactPosition();
  }

  MacroUndoEntry* getMacro() const {
    return first.macroExpansion ? first.macroExpansion :
      second.macroExpansion;
  }
};
 
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

typedef std::map<UnboxedPairLoc, std::string> patch_map;
typedef std::list<patch_map::value_type const *> hunk_queue;

class Patcher {
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
  
  void insertBefore(char const* file, UnboxedLoc const &loc,
                    std::string const &str);
  void insertBefore(CPPSourceLoc const &csl, std::string const &str,
                    int offset = 0);
private:
  void printHunkHeaderAndDeletedLines(unsigned int minLine,
                                      unsigned int maxLine,
                                      int added_lines, std::string const &file,
                                      std::ostream &ostream);

  // copies the line range from file into a specified stream and adds a prefix
  void copy(unsigned int minLine, unsigned int maxLine, std::string const &file,
	    std::ostream &ostream, std::string const &prefix);

  std::string resolveAbsolutePath(std::string const &path) const;
  //flushes a hunk at a time
  void flushQueue(hunk_queue &q);

private:
  std::ostream& out;            // where this patch is going
  std::string file;
  std::vector<std::string> lines;
  patch_map output;
  std::string dir;
  bool recursive;
};

#endif // PRCHECK_PARSER
