// hashline.h
// module for maintaining and using #line info in source files

// terminology:
//   pp source: preprocessed source, i.e. whatever had the #line
//              info sprinkled throughout it
//   orig source: original source, the files to which the #line
//                directives refer

#ifndef HASHLINE_H
#define HASHLINE_H

#include "strtable.h"       // StringTable
#include "array.h"          // ArrayStack

// map from lines in some given pp source file to lines in
// orig source files; there should be one HashLineMap object
// for each pp source file of interest
class HashLineMap {
  // dsw: Scott, I need this to be public so I can serialize it;
  // private data I can get with an accessor, but private classes are
  // a problem.  I could make the xml serialization a friend, but that
  // would make a dependency of smbase on elsa.
  public:
//  private:    // types
  // records a single #line directive
  class HashLine {
  public:
    int ppLine;              // pp source line where it appears
    int origLine;            // orig line it names
    char const *origFname;   // orig fname it names

  public:
    HashLine()
      : ppLine(0), origLine(0), origFname(NULL) {}
    HashLine(int pl, int ol, char const *of)
      : ppLine(pl), origLine(ol), origFname(of) {}
    HashLine(HashLine const &obj)
      : DMEMB(ppLine), DMEMB(origLine), DMEMB(origFname) {}
  };

public:
  char const *canonizeFilename(char const *fname);

private:    // data
  // name of the pp file; this is needed for queries to lines
  // before any #line is encountered
  string ppFname;

  // map for canonical storage of orig filenames
  StringTable filenames;

public:
// dsw: it is a real pain to do de-serialization without making this
// public

  // growable array of HashLine objects
  ArrayStack<HashLine> directives;

private:                        // more data
  // previously-added ppLine; used to verify the entries are
  // being added in sorted order
  int prev_ppLine;

public:     // funcs
  HashLineMap(rostring ppFname);
  ~HashLineMap();

  // call this time each time a #line directive is encountered;
  // successive calls must have strictly increasing values of 'ppLine'
  void addHashLine(int ppLine, int origLine, char const *origFname);

  // call this when all the #line directives have been added; this
  // consolidates the 'directives' array to reclaim any space created
  // during the growing process but that is now not needed
  void doneAdding();

  // map from pp line to orig line/file; note that queries exactly on
  // #line lines have undefined results
  void map(int ppLine, int &origLine, char const *&origFname) const;
  int mapLine(int ppLine) const;           // returns 'origLine'
  char const *mapFile(int ppLine) const;   // returns 'origFname'

  // for curiosity, find out how many unique filenames are recorded in
  // the 'filenames' dictionary
  // int numUniqueFilenames() { return filenames.size(); }

  // XML serialization only
  string &serializationOnly_get_ppFname() { return ppFname; }
  void serializationOnly_set_ppFname(string const &ppFname0) { ppFname = ppFname0; }
  ArrayStack<HashLine> &serializationOnly_get_directives() { return directives; }
};

#endif // HASHLINE_H
