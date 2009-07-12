// see License.txt for copyright and terms of use

// Implement oink control files.

#ifndef OINK_CONTROL_H
#define OINK_CONTROL_H

#include "oink_var.h"           // Variable_O
#include "cc_flags.h"
#include "strtable.h"
#include "strmap.h"
#include "str.h"
#include "astlist.h"
#include <istream>

// forward
class VariableMatcher;

// a stream that reads a file as a set of constructors for Controls
class ControlStream {
  public:
  istream &in;                  // underlying input stream
  int lineCount;                // count the lines starting at 1

  ControlStream(istream &in0)
    : in(in0)
    , lineCount(0)
  {}

  // FIX: perhaps this should be written to test for bad() etc. sec
  // 21.3.3 in Stroustrup
  // bool eof() {return in.eof();}
  operator void*() const { return in; }
  void getLine(string &line);
  static void splitLine(string const &line, string &name, string &value);
};

class OinkControl {
  public:
  enum ControlKind {
    CK_UNDEFINED,               // not a defined OinkControl
    CK_IGNORE_BODY,             // for function vars, ignore the body of the function
    CK_WEAK_DEF,                // ??? FIX: Karl, fill this in please
    CK_UNSOUNED_IF_USED,        // warn if this variable is used in the dataflow
    NUM_CONTROLKIND,            // count the number of kinds of controls
  };

  StringRef name;               // the name of the function the body of which to ignore
  StringRef file;               // the name of the file/translation unit of the function
  StringRef archive;            // the name of the archive (.qz file)
  DeclFlags visibility;         // can only be DF_STATIC or DF_EXTERN for now
  ControlKind kind;

  OinkControl(ControlKind kind0)
    : name(NULL)
    , file(NULL)
    , archive(NULL)
    , visibility(DF_EXTERN)     // seems like a natural default
    , kind(kind0)
  {}

  void initFromNameValue(string const &name, string const &value);
  bool match(VariableMatcher const &vm) const;
  bool isComplete();            // is this object completely initialized?
  void print() const;                               // for debugging
  void print(ostream &out) const;                   // for debugging
};

class Controls {
  // list of controls
  ASTList<OinkControl> controlList;

  // count them too
  int numControls[OinkControl::NUM_CONTROLKIND];

  // set of controls that were used in some way
  // TODO: change to a flag in OinkControl?
  PtrSet<OinkControl> usedControls;

public:
  // this is a pointer so that we will fail if someone attempts to use
  // the index before it has been initialized, since it won't even
  // exist
  StringRefMap<ASTList<OinkControl> > *index;

  Controls();

  void load(ControlStream &in); // load controlList from a file
  void buildIndex();            // build the index on the controlList
  void print(ostream &out);     // print in parsable form
  void makeEmptyUsedControls(); // set usedControls to the empty set
  bool allControlsUsed();       // true iff all the controls are used
  void printUnusedControls();   // print out the unused controls

  int getNumControls(OinkControl::ControlKind kind) { return numControls[kind]; }

  // should we ignore this function according to the controls?
  bool isIgnored(Variable_O *var);
  // should this function be weaker than others when linking?
  bool isWeakDef(Variable_O *var);
  // should we warn if this function is ever used in data flow?
  bool isUnsoundIfUsed(Variable_O *var);
};

namespace OinkControlStrings {
  void addArchiveName(const char *, const char *archiveName);
}

#endif // OINK_CONTROL_H
