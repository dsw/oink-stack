// cppundolog.cc            see license.txt for copyright and terms of use
// code for cppundolog.h

#include "cppundolog.h"      // this module

// main undo log structure
TailList<MacroUndoEntry> macroUndoLog;

// length of the /*>*/ macro closing comment
const int MACRO_END_LENGTH = 5;

CPPSourceLoc::CPPSourceLoc(SourceLoc loc):
  macroExpansion(NULL),
  _loc(loc),
  exactPosition(false)
{
  init(loc);
}

void CPPSourceLoc::init(SourceLoc loc) {
  MacroUndoEntry *priorMacro = NULL;

  for (TailListIterNC<MacroUndoEntry> it(macroUndoLog);
       !it.isDone() && it.data()->postStartLoc <= loc;
       it.adv()) {
   
    priorMacro = it.data();
  }

  // No macro expansions occured before loc: loc is correct
  if (!priorMacro)
    return;

  // The following can determine position on macro boundaries
  // and gives up within them
  
  SourceLoc postEndLoc;
  SourceLoc preEndLoc;
  MacroUndoEntry *macroParam = NULL;
  // stuff within parameters is real code that got displaced
  // so position calculation logic is the same 
  if (priorMacro->preStartLoc != SL_UNKNOWN
      && priorMacro->postEndLoc >= loc
      && priorMacro->isParam()) {
    // --check param range, to see if the macro param
    // starts and ends on the same line..otherwise we are 
    // probably wayyy off
    char const * file(NULL); 
    int line(0); 
    int lineEnd(0); 
    int col(0);  
    sourceLocManager->decodeLineCol(priorMacro->preStartLoc, file, line, col);
    sourceLocManager->decodeLineCol(priorMacro->preEndLoc, file, lineEnd, col);
    // return a position and macro to make sure that other calculations
    // do not accidentally cross macro boundaries 
    if (line == lineEnd) {
      preEndLoc = priorMacro->preStartLoc;
      postEndLoc = priorMacro->postStartLoc;
      macroParam = priorMacro;
    }
  } 

  if (!macroParam) {
    // work on the level of the toplevel macro
    while (priorMacro->parent) {
      priorMacro = priorMacro->parent;
    }
    preEndLoc = priorMacro->preEndLoc;
    postEndLoc = priorMacro->postEndLoc;
  
    // loc coincides with start of macro
    if (priorMacro->postStartLoc == loc) {
      this->_loc = priorMacro->preStartLoc;
      this->macroExpansion = priorMacro;
      this->exactPosition = true;
      return;
    }
    
    // loc coincides with end of macro
    if (postEndLoc == loc) {
      this->_loc = priorMacro->preEndLoc;
      this->macroExpansion = priorMacro;
      this->exactPosition = true;
      return;
    }
    
    // loc is within a macro expansion. return macro position
    if (postEndLoc > loc) {
      this->_loc = priorMacro->preStartLoc;
      this->macroExpansion = priorMacro;
      return;
    }
  }

  // loc is not within a macro expansion, calculate the correct position
  // lines should be the same as in the original loc, just the friggin cols should be diff
  char const * file(NULL); 
  int line(0); 
  int col(0);  
  int postEndLine(0);
  int postEndCol(0);
  char const *macroFile(NULL);
  sourceLocManager->decodeLineCol(loc, file, line, col);
  sourceLocManager->decodeLineCol(postEndLoc, macroFile, postEndLine,
				  postEndCol);
  // if macro is on the same line as loc, then the column must've been affected
  // use original loc if that's not the case
  if (file != macroFile || line != postEndLine)
    return;

  int preEndCol(0);
  // override the line with one from macro
  sourceLocManager->decodeLineCol(preEndLoc, file, line, preEndCol);
  col = preEndCol + col - postEndCol;
  if (!macroParam)
    col -= MACRO_END_LENGTH;
  
  SourceLoc correctLoc = sourceLocManager->encodeLineCol(file,
							 line, col);
  this->_loc = correctLoc;
  this->macroExpansion = macroParam;
  this->exactPosition = true;
}
// EOF
