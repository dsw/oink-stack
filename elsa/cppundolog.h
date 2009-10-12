// ccundolog.h            see license.txt for copyright and terms of use
// data structures used for reversing CPP

#ifndef CCUNDOLOG_H
#define CCUNDOLOG_H

#include "taillist.h"       // TailList
#include "strobjdict.h"     // StringObjDict
#include "srcloc.h"
#include "taillist.h"

class MacroUndoEntry;

class MacroDefinition {
public:
  SourceLoc fromLoc;
  SourceLoc toLoc;
  const char *name;
  MacroDefinition(const char *name, SourceLoc fromLoc, SourceLoc toLoc):
  fromLoc(fromLoc),
    toLoc(toLoc),
    name(name)
    {
    }
};

class MacroUndoEntry {
public:
  // post-expansion start & end position
  // helps determine if an ast node is part of a macro
  SourceLoc postStartLoc;
  SourceLoc postEndLoc;

  // pre-expansion positions of the macro
  SourceLoc preStartLoc;
  SourceLoc preEndLoc;

  // macro name
  const char *name;
  // parent macro
  MacroUndoEntry *parent;
  
  TailList<MacroDefinition> params;

  MacroUndoEntry(SourceLoc postStartLoc, SourceLoc preStartLoc, SourceLoc preEndLoc,
		 const char *name, MacroUndoEntry *parent):
    postStartLoc(postStartLoc),
    postEndLoc(SL_UNKNOWN),
    preStartLoc(preStartLoc),
    preEndLoc(preEndLoc),
    name(name),
    parent(parent)
  {
  }

  bool isParam() {
    return strchr(name, ':') != NULL;
  }
 
};

class CPPSourceLoc {
public:
  MacroUndoEntry *macroExpansion;
  CPPSourceLoc(SourceLoc loc);

  bool hasExactPosition() const {
    return _loc != SL_UNKNOWN && (!macroExpansion || exactPosition);
  }

  SourceLoc loc() const {
    return _loc;
  }

  // overriding loc implies that the position is now exact
  void overrideLoc(SourceLoc loc) {
    _loc = loc;
    exactPosition = true;
  } 
private:
  // gdb wont let me debug the constructor
  void init(SourceLoc loc);
  SourceLoc _loc;
  // can be true when a position within a macro can be traced 
  // to an ectual position(ie at start/end or in a param).
  // This flag is only valid when macroExpansion != NULL
  // because the position is exact when macroExpansion == NULL
  bool exactPosition;
};

// main undo log structure
extern TailList<MacroUndoEntry> macroUndoLog;

#endif // CCUNDOLOG_H
