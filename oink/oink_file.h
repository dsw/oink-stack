// see License.txt for copyright and terms of use

// Implement handling input files for analysis.

#ifndef OINK_FILE_H
#define OINK_FILE_H

#include "astlist.h"
#include <string>
#include <stdio.h>
#include <strings.h>

class File;

// superclass for File and ProgramFile
class InputFile {
  public:
  std::string name;
  protected:                    // prevent from being instantiated directly
  InputFile(std::string const &name0)
    : name(name0)
  {}
  public:
  virtual ~InputFile() {}
  virtual void dump() const = 0;
  virtual void appendSelfIntoList(ASTList<File> &list) const = 0;
};

// the name of a file to use as input directly
class File : public InputFile {
  public:
  File(std::string const &name0) : InputFile(name0) {}
  void dump() const;
  void appendSelfIntoList(ASTList<File> &list) const;
};

// the name of a file that contains a list of files
class ProgramFile : public InputFile {
  public:
  ProgramFile(std::string const &name0) : InputFile(name0) {}
  void dump() const;
  void appendSelfIntoList(ASTList<File> &list) const;
};

// clean up a raw string and return it if it makes a sensible name;
// otherwise return NULL
char *trimFilename(char *filename, int bufsize);

static inline
char const *getSuffix0(char const *name) {
  xassert(name);
  char const *suff = rindex(name, '.');
  return suff ? suff : "";
}

char const *getSuffix(char const *name);

inline bool isAbsolutePathname(char const* filename) { return filename[0] == '/'; }

#endif // OINK_FILE_H
