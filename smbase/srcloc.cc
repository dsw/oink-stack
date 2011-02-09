// srcloc.cc            see license.txt for copyright and terms of use
// code for srcloc.h

#include "srcloc.h"     // this module
#include "autofile.h"   // AutoFILE
#include "array.h"      // ArrayStack
#include "syserr.h"     // xsyserror
#include "trace.h"      // traceProgress
#include "hashline.h"   // HashLineMap

#include <stdio.h>      // fprintf
#include <string.h>     // memcpy


// this parameter controls the frequency of Markers in
// the marker index; lower period makes the index
// faster but use more space
enum { MARKER_PERIOD = 100 };    // 100 is about a 10% overhead


// given an ArrayStack, copy the data into a ptr/len pair
template <class T>
void extractArray(T *&array, int &size, ArrayStack<T> const &src)
{
  size = src.length();
  array = new T[size];
  memcpy(array, src.getArray(), size * sizeof(T));
}


// ------------------------- File -----------------------
void addLineLength(ArrayStack<unsigned char> &lengths, int len)
{
  while (len >= UCHAR_MAX) {
    // add a long-line marker, which represents UCHAR_MAX chars of input
    lengths.push(UCHAR_MAX);
    len -= UCHAR_MAX;
  }

  // add the short count at the end
  lengths.push((unsigned char)len);
}


SourceLocManager::File::File(char const *n, SourceLoc aStartLoc)
  : name(n),
    startLoc(aStartLoc),     // assigned by SourceLocManager
    hashLines(NULL),

    // valid marker/col for the first char in the file
    marker(0, 1, 0),
    markerCol(1),
    erroredNumLines(false)
{
  // dsw: I want a way to make sure that we never look for a file.
  xassert(sourceLocManager->mayOpenFiles);

  // open in binary mode since it's too unpredictable whether
  // the lower level (e.g. cygwin) will do CRLF translation,
  // and whether that will be done consistently, if text mode
  // is requested
  AutoFILE fp(toCStr(name), "rb");

  // the buffering that FILE would do would be wasted, so
  // make it unbuffered (if this causes a problem on some
  // system it can be commented-out)
  setbuf(fp, NULL);

  // These are growable versions of the indexes.  They will be
  // discarded after the file has been read.  They keep track of their
  // own 'next index' values.  I chose to use a growable array instead
  // of making two passes over the file to reduce i/o.
  ArrayStack<unsigned char> lineLengths;
  ArrayStack<Marker> index;

  // put a marker at the start for uniformity
  index.push(Marker(0, 1, 0));

  // how many lines to go before I insert the next marker
  int indexDelay = MARKER_PERIOD;

  // where I am in the file
  int charOffset = 0;
  int lineNum = 1;
  int lineLen = 0;       // length of current line, so far

  // read the file, computing information about line lengths
  enum { BUFLEN=8192 };
  char buf[BUFLEN];
  for (;;) {
    // read a buffer of data
    int len = fread(buf, 1, BUFLEN, fp);
    if (len < 0) {
      xsyserror("fread", name);
    }
    if (len==0) {
      break;
    }

    // the code that follows can be seen as abstracting the data
    // contained in buf[0] through buf[len-1] and adding that
    // information to the summary variables above
    char const *start = buf;      // beginning of unaccounted-for chars
    char const *p = buf;          // scan pointer
    char const *end = buf+len;    // end of unaccounted-for chars

    // loop over the lines in 'buf'
    while (start < end) {
      // scan to the next newline
      xassert(start == p);
      while (p<end && *p!='\n') {
        p++;
      }
      if (p==end) {
        break;
      }
      xassert(*p == '\n');

      // account for [start,p)
      charOffset += p-start;
      lineLen += p-start;
      start = p;

      // account for the newline at '*p'
      addLineLength(lineLengths, lineLen);
      charOffset++;
      lineNum++;
      lineLen = 0;

      p++;
      start++;

      if (--indexDelay == 0) {
        // insert a marker to remember this location
        index.push(Marker(charOffset, lineNum, lineLengths.length()));
        indexDelay = MARKER_PERIOD;
      }
    }

    // move [start,p) into 'lineLen'
    charOffset += p-start;
    lineLen += p-start;
    start = p;
    xassert(start == end);
  }

  // handle the last line; in the usual case, where a newline is
  // the last character, the final line will have 0 length
  addLineLength(lineLengths, lineLen);

  // move computed information into 'this'
  this->numChars = charOffset;
  this->numLines = lineNum;
  xassert(numLines >= 1);
  this->avgCharsPerLine = numChars / numLines;

  extractArray(this->lineLengths, this->lineLengthsSize, lineLengths);
  extractArray(this->index, this->indexSize, index);

  selfCheck();

  // 'fp' closed by the AutoFILE
}

SourceLocManager::File::~File()
{
  if (hashLines) {
    delete hashLines;
  }
  delete[] lineLengths;
  delete[] index;
}

SourceLocManager::File::File(FileData *fileData, SourceLoc aStartLoc)
  : name(fileData->name),
    startLoc(aStartLoc),        // assigned by SourceLocManager
//      hashLines(fileData->hashLines),
    hashLines(NULL),            // dsw: initialized later

    // valid marker/col for the first char in the file
    marker(0, 1, 0),
    markerCol(1),
    erroredNumLines(false)
{
  xassert(fileData->complete());

  numChars = fileData->numChars;
  numLines = fileData->numLines;
  xassert(numLines >= 1);
  avgCharsPerLine = numChars / numLines;

  extractArray(lineLengths, lineLengthsSize, *(fileData->lineLengths));

  // make a Marker every MARKER_PERIOD lines
  ArrayStack<Marker> index;
  index.push(Marker(0, 1, 0));
  int indexDelay = MARKER_PERIOD;
  int numChars0 = 0;
  int numLines0 = 1;
  for(int i=0; i<lineLengthsSize; ++i) {
    int len0 = lineLengths[i];
    numChars0 += len0;
    if (len0 != UCHAR_MAX) {
      // a new line
      ++numLines0;
      // Scott counts newlines as out-of-band, not counted in the line
      // lenth, so add it back in.
      ++numChars0;
      if (--indexDelay == 0 && i+1 < lineLengthsSize) {
        // insert a marker to remember this location
        index.push(Marker(numChars0, numLines0, i+1));
        indexDelay = MARKER_PERIOD;
      }
    }
  }
  // dsw: These assertions demonstrate that something is wrong with
  // Scott's other File ctor that counts the characters by reading in
  // the file.  If you use a file that had no newlines in it and read
  // it in and print it out again, the numChars serialized in the XML
  // will be twice the length of the file.
  //
  // last newline doesn't count or something??
//    --numChars0;
//    --numLines0;
//    xassert(numChars0 == numChars);
//    xassert(numLines0 == numLines);

  extractArray(this->index, this->indexSize, index);

  selfCheck();
}


int SourceLocManager::File::lineLengthSum() const
{
  int sum = 0;
  for (int i=0; i<lineLengthsSize; i++) {
    sum += lineLengths[i];
  }
  return sum;
}

void SourceLocManager::File::selfCheck() const
{
  xassert(lineLengthSum() + numLines-1 == numChars);

  // check the markers
  int charOffset = 0;   // current character offset
  int lineNum = 1;      // current line number
  int m = 1;            // current marker to validate
  bool foundMovableMarker = false;
  for (int i=0; i<lineLengthsSize; i++) {
    if (m < indexSize) {
      if (index[m].charOffset <= charOffset) {
        // if we just reached or exceeded the offset of marker
        // 'm', check its stats
        xassert(index[m].charOffset == charOffset &&
                index[m].lineOffset == lineNum &&
                index[m].arrayOffset == i);

        // ready to check next marker
        m++;
      }
    }

    // I'm only about 90% sure this is the right invariant...
    if (marker.charOffset-(markerCol-1) == charOffset) {
      foundMovableMarker = true;
      xassert(marker.lineOffset == lineNum &&
              marker.arrayOffset == i);
    }

    charOffset += lineLengths[i];
    if (lineLengths[i] < UCHAR_MAX) {
      // account for the newline character
      charOffset++;
      lineNum++;
    }
  }
  xassert(foundMovableMarker);
  xassert(m == indexSize);

  // marker offsets should be in increasing order
  for (m=0; m<(indexSize-1); m++) {
    xassert(index[m].charOffset < index[m+1].charOffset);
  }
}


void SourceLocManager::File::resetMarker()
{
  marker.charOffset = 0;
  marker.lineOffset = 1;
  marker.arrayOffset = 0;
  markerCol = 1;
}


// it's conceivable gcc is smart enough to recognize
// the induction variable, if I inline this..
inline void SourceLocManager::File::advanceMarker()
{
  int len = (int)lineLengths[marker.arrayOffset];
  if (len < UCHAR_MAX) {
    // normal length line
    marker.charOffset += len+1;     // +1 for newline
    marker.lineOffset++;
    marker.arrayOffset++;
    markerCol = 1;
  }
  else {
    // fragment of a long line, representing UCHAR_MAX characters
    marker.charOffset += UCHAR_MAX;
    marker.arrayOffset++;
    markerCol += UCHAR_MAX;
  }
}


int SourceLocManager::File::lineToChar(int lineNum)
{
  xassert(1 <= lineNum);
  // xassert(lineNum <= numLines);

  // If we encounter an invalid line number, don't abort fatally, just say
  // line numbers will be wrong.  This happens often when a cpp'ed file is
  // modified.
  if (erroredNumLines) {
    return 0;
  }

  if (lineNum > numLines) {
    fprintf(stderr,
            "Error: invalid line number %s:%d (only %d lines exist).\n"
            "       Line numbers will be incorrect.\n",
            name.c_str(), lineNum, numLines);
    if (tolerateHashlineErrors) {
      erroredNumLines = true;
      return 0;
    } else {
      throw xBase("Invalid hashline numbers found (use '-tr tolerateHashlineErrors' to ignore).");
    }
  }

  // check to see if the marker is already close
  if (marker.lineOffset <= lineNum &&
                           lineNum < marker.lineOffset + MARKER_PERIOD) {
    // use the marker as-is
  }
  else {
    // do a binary search on the index to find the marker whose
    // lineOffset is closest to 'lineNum' without going over
    int low = 0;              // lowest index under consideration
    int high = indexSize-1;   // highest index under consideration
    while (low < high) {
      // check the midpoint (round up to ensure progress when low+1 == high)
      int mid = (low+high+1)/2;
      if (index[mid].lineOffset > lineNum) {
        // too high
        high = mid-1;
      }
      else {
        // too low or just right
        low = mid;
      }
    }

    // copy this index marker into our primary marker
    marker = index[low];
    markerCol = 1;            // all index markers implicitly have column 1
  }

  xassert(marker.lineOffset <= lineNum);

  // move the marker down the array until it arrives at
  // the desired line
  while (marker.lineOffset < lineNum) {
    advanceMarker();
  }

  // make sure we never go beyond the end of the array
  xassert(marker.arrayOffset < lineLengthsSize);

  // if we didn't move the marker, we might not be in column 1
  return marker.charOffset - (markerCol-1);
}


int SourceLocManager::File::lineColToChar(int lineNum, int col)
{
  // use the above function first
  int offset = lineToChar(lineNum);

  // now, we use an property established by the previous function:
  // the marker points at the line of interest, possibly offset from
  // the line start by 'markerCol-1' places
  if (col <= markerCol) {
    // the column we want is not even beyond the marker's column,
    // so it's surely not beyond the end of the line, so do the
    // obvious thing
    return offset + (col-1);
  }

  // we're at least as far as the marker; move the offset up to
  // this far, and subtract from 'col' so it now represents the
  // number of chars yet to traverse on this line
  offset = marker.charOffset;
  col -= markerCol;       // 'col' is now 0-based

  // march to the end of the line, looking for either the end or a
  // line length component which goes beyond 'col'; I don't move the
  // marker itself out of concern for preserving the locality of
  // future accesses
  int index = marker.arrayOffset;
  for (;;) {
    int len = (int)lineLengths[index];
    if (col <= len) {
      // 'col' doesn't go beyond this component, we're done
      // (even if len==UCHAR_MAX it still works)
      return offset + col;
    }
    if (len < UCHAR_MAX) {
      // the line ends here, truncate and we're done
      SourceLocManager::shortLineCount++;
      return offset + len;
    }

    // the line continues
    xassertdb(len == UCHAR_MAX);

    col -= UCHAR_MAX;
    offset += UCHAR_MAX;
    xassertdb(col > 0);

    index++;
    xassert(index < lineLengthsSize);
  }
}


void SourceLocManager::File::charToLineCol(int offset, int &line, int &col)
{
  xassert(0 <= offset && offset <= numChars);

  // check if the marker is close enough
  if (marker.charOffset <= offset &&
                           offset < marker.charOffset + MARKER_PERIOD*avgCharsPerLine) {
    // use as-is
  }
  else {
    // binary search, like above
    int low = 0;
    int high = indexSize-1;
    while (low < high) {
      // check midpoint
      int mid = (low+high+1)/2;
      if (index[mid].charOffset > offset) {
        high = mid-1;
      }
      else {
        low = mid;
      }
    }

    // copy this marker
    marker = index[low];
    markerCol = 1;
  }

  xassert(marker.charOffset <= offset);

  // move the marker until it's within one spot of moving
  // beyond the offset
  while (marker.charOffset + lineLengths[marker.arrayOffset] < offset) {
    advanceMarker();
  }

  // make sure we never go beyond the end of the array
  xassert(marker.arrayOffset < lineLengthsSize);

  // read off line/col
  line = marker.lineOffset;
  col = markerCol + (offset - marker.charOffset);
}


void SourceLocManager::File::addHashLine
  (int ppLine, int origLine, char const *origFname)
{
  if (!hashLines) {
    hashLines = new HashLineMap(name);
  }
  hashLines->addHashLine(ppLine, origLine, origFname);
}

void SourceLocManager::File::doneAdding()
{
  if (hashLines) {
    hashLines->doneAdding();
  }
  else {
    // nothing to consolidate, the NULL pointer is valid and
    // will cause the map to be ignored, so do nothing
  }
}


// ----------------------- StaticLoc -------------------
SourceLocManager::StaticLoc::~StaticLoc()
{}


// ----------------------- SourceLocManager -------------------
int SourceLocManager::shortLineCount = 0;
bool SourceLocManager::tolerateHashlineErrors = false;

SourceLocManager *sourceLocManager = NULL;


SourceLocManager::SourceLocManager()
  : files(),
    recent(NULL),
    statics(),
    nextLoc(toLoc(1)),
    nextStaticLoc(toLoc(0)),
    mayOpenFiles(true),
    maxStaticLocs(100),
    useHashLines(true),
    useOriginalOffset(true)
{
  if (!sourceLocManager) {
    sourceLocManager = this;
  }

  makeFirstStatics();
}

SourceLocManager::~SourceLocManager()
{
  if (sourceLocManager == this) {
    sourceLocManager = NULL;
  }
}


void SourceLocManager::reset()
{
  files.deleteAll();
  recent = NULL;
  statics.deleteAll();
  nextLoc = toLoc(1);
  nextStaticLoc = toLoc(0);
  makeFirstStatics();
}

void SourceLocManager::makeFirstStatics()
{
  // slightly clever: treat SL_UNKNOWN as a static
  SourceLoc u = encodeStatic(StaticLoc("<noloc>", 0,1,1));
  xassert(u == SL_UNKNOWN);
  PRETEND_USED(u);     // silence warning in case xasserts are turned off

  // similarly for SL_INIT
  u = encodeStatic(StaticLoc("<init>", 0,1,1));
  xassert(u == SL_INIT);
  PRETEND_USED(u);
}


// find it, or return NULL
SourceLocManager::File *SourceLocManager::findFile(char const *name)
{
  if (!this) {
    // it's quite common to forget to do this, and this function is
    // almost always the one which segfaults in that case, so I'll
    // make the error message a bit nicer to save a trip through
    // the debugger
    xfailure("you have to create a SourceLocManager in your main() function");
  }

  if (recent && recent->name.equals(name)) {
    return recent;
  }

  FOREACH_OBJARRAYSTACK_NC(File, files, iter) {
    if (iter.data()->name.equals(name)) {
      return recent = iter.data();
    }
  }

  return NULL;
}

// find it or make it
SourceLocManager::File *SourceLocManager::getFile(char const *name)
{
  File *f = findFile(name);
  if (!f) {
    // read the file
    f = new File(name, nextLoc);
    files.append(f);

    // bump 'nextLoc' according to how long that file was,
    // plus 1 so it can own the position equal to its length
    nextLoc = toLoc(f->startLoc + f->numChars + 1);
  }

  return recent = f;
}


// load a file from a FileData object
void SourceLocManager::loadFile(FileData *fileData)
{
  xassert(fileData);
  // we should be loading a new file; dsw: I think this should remain
  // an assertion failure instead of being a user error because the
  // client code can check this before calling into the
  // SourceLocManager and provide a better error message there
  xassert(!findFile(fileData->name.c_str()));

  // finish off the fileData->hashLines object; FIX: there is probably
  // a better place for this, but I'm not sure where
//    if (fileData->hashLines) {
//      FOREACH_ARRAYSTACK_NC(HashLineMap::HashLine, fileData->hashLines->directives, iter) {
//        HashLineMap::HashLine *line = iter.data();
//        char const * const origFname = line->origFname;
//        StringObjDict<string> &filenames = fileData->hashLines->serializationOnly_get_filenames();
//        // load the 'filenames' dictionary with the origFnames of the FileDatas.
//        string *canon = filenames.queryif(origFname);
//        if (!canon) {
//          canon = new string(origFname);
//          filenames.add(origFname, canon);
//        }
//        line->origFname = canon->c_str();
//      }
//    }

  // convert the FileData object to a File
  File *f = new File(fileData, nextLoc);
  files.append(f);
  if (fileData->hashLines) {
    FOREACH_ARRAYSTACK_NC(HashLineMap::HashLine, fileData->hashLines->directives, iter) {
      HashLineMap::HashLine *line = iter.data();
      f->addHashLine(line->ppLine, line->origLine, line->origFname);
    }
    f->doneAdding();
  }

  // bump 'nextLoc' according to how long that file was,
  // plus 1 so it can own the position equal to its length
  nextLoc = toLoc(f->startLoc + f->numChars + 1);
}


SourceLoc SourceLocManager::encodeOffset(
  char const *filename, int charOffset)
{
  xassert(charOffset >= 0);

  File *f = getFile(filename);
  xassert(charOffset <= f->numChars);
  return toLoc(f->startLoc + charOffset);
}


SourceLoc SourceLocManager::encodeLineCol(
  char const *filename, int line, int col)
{
  xassert(line >= 1);
  xassert(col >= 1);

  File *f = getFile(filename);

  // map from a line number to a char offset
  int charOffset = f->lineColToChar(line, col);
  return toLoc(toInt(f->startLoc) + charOffset);
}


SourceLoc SourceLocManager::encodeStatic(StaticLoc const &obj)
{
  if (-toInt(nextStaticLoc) == maxStaticLocs) {
    // Each distinct static location should correspond to a single
    // place in the source code.  If one place in the source is creating
    // a given static location over and over, that's bad because it
    // quickly leads to poor performance when storing and decoding them.
    // Instead, make one and re-use it.
    //
    // If this message is being printed because the program is just
    // really big and has lots of distinct static locations, then you
    // can increase maxStaticLocs manually.
    fprintf(stderr,
      "Warning: You've created %d static locations, which is symptomatic\n"
      "of a bug.  See %s, line %d.\n",
      maxStaticLocs, __FILE__, __LINE__);
  }

  // save this location
  statics.append(new StaticLoc(obj));

  // return current index, yield next
  SourceLoc ret = nextStaticLoc;
  nextStaticLoc = toLoc(toInt(ret) - 1);
  return ret;
}


SourceLocManager::File *SourceLocManager::findFileWithLoc(SourceLoc loc)
{
  static int count1 = 0;
  ++count1;

  // check cache
//    if (recent && recent->hasLoc(loc)) {
//      return recent;
//    }

  // quarl 2006-05-21
  //    Used to use a linear search through a linked list; now we do a O(log
  //    N) binary search through an array.

  // // iterative walk
  // FOREACH_OBJARRAYSTACK_NC(File, files, iter) {
  //   if (iter.data()->hasLoc(loc)) {
  //     return recent = iter.data();
  //   }
  // }

  // binary search
  int left  = 0;                  // inclusive lower bound
  int right = files.length() - 1; // inclusive upper bound

  while (left <= right) {
    int mid = int((right-left)/2) + left;
    File *file = files[mid];
    int cmp = file->cmpLoc(loc);

    if (cmp == 0) {
      // xassert(file->hasLoc(loc));
      return file;
    } else if (cmp > 0) {
      right = mid - 1;
    } else if (cmp < 0) {
      left = mid + 1;
    }
  }

  // the user gave me a value that I never made!
  xfailure("invalid source location");
  return NULL;    // silence warning
}


SourceLocManager::StaticLoc const *SourceLocManager::getStatic(SourceLoc loc)
{
  int index = -toInt(loc);
  return statics.nthC(index);
}


void SourceLocManager::decodeOffset(
  SourceLoc loc, char const *&filename, int &charOffset)
{
  decodeOffset_explicitHL(loc, filename, charOffset, this->useHashLines);
}

void SourceLocManager::decodeOffset_explicitHL(
  SourceLoc loc, char const *&filename, int &charOffset,
  bool localUseHashLines)
{
  // check for static
  if (isStatic(loc)) {
    StaticLoc const *s = getStatic(loc);
    filename = s->name.c_str();
    charOffset = s->offset;
    return;
  }

  File *f = findFileWithLoc(loc);
  filename = f->name.c_str();
  charOffset = toInt(loc) - toInt(f->startLoc);

  if (localUseHashLines && f->hashLines) {
    // we can't pass charOffsets directly through the #line map, so we
    // must first map to line/col and then back to charOffset after
    // going through the map

    // map to a pp line/col
    int ppLine, ppCol;
    f->charToLineCol(charOffset, ppLine, ppCol);

    // map to original line/file
    int origLine;
    char const *origFname;
    f->hashLines->map(ppLine, origLine, origFname);

    // dsw: we want to avoid looking for the original file if we just
    // need *some* offset, not *the* original offset; thanks to Matt
    // Harren for pointing out this subtle trick
    if (this->useOriginalOffset) {
      // get a File for the original file; this opens that file
      // and scans it for line boundaries
      File *orig = getFile(origFname);

      // use that map to get an offset, truncating columns that are
      // beyond the true line ending (happens due to macro expansion)
      charOffset = orig->lineColToChar(origLine, ppCol);
    }

    // filename is whatever #line said
    filename = origFname;
  }
}


void SourceLocManager::decodeLineCol(
  SourceLoc loc, char const *&filename, int &line, int &col)
{
  decodeLineCol_explicitHL(loc, filename, line, col, this->useHashLines);
}

void SourceLocManager::decodeLineCol_explicitHL(
  SourceLoc loc, char const *&filename, int &line, int &col,
  bool localUseHashLines)
{
  if (!this) {
    // didn't initialize a loc manager.. but maybe we can survive?
    if (loc == SL_UNKNOWN) {
      filename = "<noloc>";
      line = 1;
      col = 1;
      return;
    }
    else {
      xfailure("you have to create a SourceLocManager in your main() function");
    }
  }

  // check for static
  if (isStatic(loc)) {
    StaticLoc const *s = getStatic(loc);
    filename = s->name.c_str();
    line = s->line;
    col = s->col;
    return;
  }

  File *f = findFileWithLoc(loc);
  filename = f->name.c_str();
  int charOffset = toInt(loc) - toInt(f->startLoc);

  f->charToLineCol(charOffset, line, col);

  if (localUseHashLines && f->hashLines) {
    // use the #line map to determine a new file/line pair; simply
    // assume that the column information is still correct, though of
    // course in C, due to macro expansion, it isn't always
    f->hashLines->map(line, line, filename);
  }
}


char const *SourceLocManager::getFile(SourceLoc loc, bool localUseHashLines)
{
  char const *name;
  int ofs;
  decodeOffset_explicitHL(loc, name, ofs, localUseHashLines);
  return name;
}


int SourceLocManager::getOffset(SourceLoc loc)
{
  char const *name;
  int ofs;
  decodeOffset(loc, name, ofs);
  return ofs;
}

int SourceLocManager::getOffset_nohashline(SourceLoc loc)
{
  char const *name;
  int ofs;
  decodeOffset_explicitHL(loc, name, ofs, false);
  return ofs;
}

int SourceLocManager::getLine(SourceLoc loc)
{
  char const *name;
  int line, col;
  decodeLineCol(loc, name, line, col);
  return line;
}


int SourceLocManager::getCol(SourceLoc loc)
{
  char const *name;
  int line, col;
  decodeLineCol(loc, name, line, col);
  return col;
}


string SourceLocManager::getString(SourceLoc loc)
{
  return getString_explicitHL(loc, this->useHashLines);
}

string SourceLocManager::getString_explicitHL(SourceLoc loc, bool localUseHashLines)
{
  char const *name;
  int line, col;
  decodeLineCol_explicitHL(loc, name, line, col, localUseHashLines);

  return stringc << name << ":" << line << ":" << col;
}

string SourceLocManager::getLCString(SourceLoc loc)
{
  char const *name;
  int line, col;
  decodeLineCol(loc, name, line, col);

  return stringc << line << ":" << col;
}


string locToStr(SourceLoc sl)
{
  return sourceLocManager->getString(sl);
}


// -------------------------- test code ----------------------
#ifdef TEST_SRCLOC

#include "test.h"        // USUAL_MAIN
#include "strtokp.h"     // StrtokParse

#include <stdlib.h>      // rand, exit, system

SourceLocManager mgr;
int longestLen=0;

// given a location, decode it into line/col and then re-encode,
// and check that the new encoding matches the old
void testRoundTrip(SourceLoc loc)
{
  char const *fname;
  int line, col;
  mgr.decodeLineCol(loc, fname, line, col);

  if (col > longestLen) {
    longestLen = col;
  }

  SourceLoc loc2 = mgr.encodeLineCol(fname, line, col);

  xassert(loc == loc2);
}


// location in SourceLoc and line/col
class BiLoc {
public:
  int line, col;
  SourceLoc loc;
};


// given a file, compute SourceLocs throughout it and verify
// that round-trip encoding works
void testFile(char const *fname)
{
  // dsw: I want a way to make sure that we never look for a file.
  xassert(sourceLocManager->mayOpenFiles);

  // find the file's length
  int len;
  {
    AutoFILE fp(fname, "rb");

    fseek(fp, 0, SEEK_END);
    len = (int)ftell(fp);
    std::cout << "length of " << fname << ": " << len << std::endl;
  }

  // get locations for the start and end
  SourceLoc start = mgr.encodeOffset(fname, 0);
  SourceLoc end = mgr.encodeOffset(fname, len);

  // check expectations for start
  xassert(mgr.getLine(start) == 1);
  xassert(mgr.getCol(start) == 1);

  // test them
  testRoundTrip(start);
  testRoundTrip(end);

  // temporary
  //testRoundTrip((SourceLoc)11649);

  BiLoc *bi = new BiLoc[len+1];
  char const *dummy;

  // test all positions, forward sequential; also build the
  // map for the random test; note that 'len' is considered
  // a valid source location even though it corresponds to
  // the char just beyond the end
  int i;
  for (i=0; i<=len; i++) {
    SourceLoc loc = mgr.encodeOffset(fname, i);
    testRoundTrip(loc);

    bi[i].loc = loc;
    mgr.decodeLineCol(loc, dummy, bi[i].line, bi[i].col);
  }

  // backward sequential
  for (i=len; i>0; i--) {
    SourceLoc loc = mgr.encodeOffset(fname, i);
    testRoundTrip(loc);
  }

  // random access, both mapping directions
  for (i=0; i<=len; i++) {
    int j = rand()%(len+1);
    int dir = rand()%2;

    if (dir==0) {
      // test loc -> line/col map
      int line, col;
      mgr.decodeLineCol(bi[j].loc, dummy, line, col);
      xassert(line == bi[j].line);
      xassert(col == bi[j].col);
    }
    else {
      // test line/col -> loc map
      SourceLoc loc = mgr.encodeLineCol(fname, bi[j].line, bi[j].col);
      xassert(loc == bi[j].loc);
    }
  }

  delete[] bi;
}


// write a file with the given contents, and call 'testFile' on it
void testFileString(char const *contents)
{
  {
    AutoFILE fp("srcloc.tmp", "w");
    int written = fwrite(contents, 1, strlen(contents), fp);
    xassert(written == (int)strlen(contents));
  }

  testFile("srcloc.tmp");

  // since I keep using "srcloc.tmp" over and over, I need to reset
  // the manager between attempts since otherwise it thinks it already
  // knows the line lengths
  mgr.reset();
}


// decode with given expectation, complain if it doesn't match
void expect(SourceLoc loc, char const *expFname, int expLine, int expCol)
{
  char const *fname;
  int line, col;
  mgr.decodeLineCol(loc, fname, line, col);

  if (0!=strcmp(fname, expFname) ||
      line != expLine ||
      col != expCol) {
    printf("expected %s:%d:%d, but got %s:%d:%d\n",
           expFname, expLine, expCol,
           fname, line, col);
    exit(2);
  }
}


// should this be exported?
string locString(char const *fname, int line, int col)
{
  return stringc << fname << ":" << line << ":" << col;
}


void buildHashMap(SourceLocManager::File *pp, char const *fname, int &expanderLine)
{
  expanderLine = 0;

  // dsw: I want a way to make sure that we never look for a file.
  xassert(sourceLocManager->mayOpenFiles);

  AutoFILE fp(fname, "rb");

  enum { SZ=256 };
  char buf[SZ];
  int ppLine=0;
  while (fgets(buf, SZ, fp)) {
    if (buf[strlen(buf)-1] == '\n') {
      ppLine++;
    }

    if (0==memcmp(buf, "int blah_de_blah", 16)) {
      expanderLine = ppLine;
    }

    if (buf[0]!='#') continue;

    // break into tokens at whitespace (this isn't exactly
    // right, because the file names can have quoted spaces,
    // but it will do for testing purposes)
    StrtokParse tok(buf, " \r\n");
    if (tok < 3) continue;

    int origLine = atoi(tok[1]);
    char const *tok2 = tok[2];
    string origFname = substring(tok2+1, strlen(tok2)-2);  // remove quotes
    pp->addHashLine(ppLine, origLine, origFname.c_str());
  }
  pp->doneAdding();
}


void testHashMap()
{
  // run the preprocessor
  if (0!=system("gcc -E -DTEST_SRCLOC srcloc.test.cc >srcloc.tmp 2>/dev/null")) {
    xbase("failed to preprocess srcloc.test.cc; the command that failed was:\n"
          "  gcc -E -DTEST_SRCLOC srcloc.test.cc >srcloc.tmp");
  }

  SourceLocManager::File *pp = mgr.getInternalFile("srcloc.tmp");
  SourceLocManager::File *orig = mgr.getInternalFile("srcloc.test.cc");

  // read srcloc.tmp and install the hash maps
  int expanderLine=0;
  buildHashMap(pp, "srcloc.tmp", expanderLine);

  // the 2nd line in the pp source should correspond to the
  // first line in the orig src
  // update: this doesn't work with all preprocessors, and I'm
  // confident in the implementation now, so I'll turn this off
  //SourceLoc lineTwo = mgr.encodeLineCol("srcloc.tmp", 2, 1);
  //expect(lineTwo, "srcloc.cc", 1,1);

  // print decodes of first several lines (including those that
  // are technically undefined because they occur on #line lines)
  int ppLine;
  for (ppLine = 1; ppLine < 10; ppLine++) {
    SourceLoc loc = mgr.encodeLineCol("srcloc.tmp", ppLine, 1);
    std::cout << "ppLine " << ppLine << ": " << toString(loc) << std::endl;
  }

  // similar for last few lines
  for (ppLine = pp->numLines - 4; ppLine <= pp->numLines; ppLine++) {
    SourceLoc loc = mgr.encodeLineCol("srcloc.tmp", ppLine, 1);
    std::cout << "ppLine " << ppLine << ": " << toString(loc) << std::endl;
  }

  // see how the expander line behaves
  if (!expanderLine) {
    std::cout << "didn't find expander line!\n";
    exit(2);
  }
  else {
    SourceLoc loc = mgr.encodeLineCol("srcloc.tmp", expanderLine, 1);
    std::cout << "expander column 1: " << toString(loc) << std::endl;

    // in the pp file, I can advance the expander horizontally a long ways;
    // this should truncate to column 9
    loc = advCol(loc, 20);

    char const *fname;
    int offset;
    mgr.decodeOffset(loc, fname, offset);
    std::cout << "expander column 21: " << fname << ", offset " << offset << std::endl;
    xassert(0==strcmp(fname, "srcloc.test.cc"));

    // map that to line/col, which should show the truncation
    int line, col;
    orig->charToLineCol(offset, line, col);
    std::cout << "expander column 21: " << locString(fname, line, col) << std::endl;
    if (col != 9 && col != 10) {
      // 9 is for LF line endings, 10 for CRLF
      std::cout << "expected column 9 or 10!\n";
      exit(2);
    }
  }
}


void testHashMap2()
{
  SourceLocManager::File *pp = mgr.getInternalFile("srcloc.test2.cc");

  int expanderLine=0;
  buildHashMap(pp, "srcloc.test2.cc", expanderLine);

  for (int ppLine = 1; ppLine <= pp->numLines; ppLine++) {
    SourceLoc loc = mgr.encodeLineCol("srcloc.test2.cc", ppLine, 1);
    std::cout << "ppLine " << ppLine << ": " << toString(loc) << std::endl;
  }
}


void entry(int argc, char ** /*argv*/)
{
  xBase::logExceptions = false;
  traceAddSys("progress");
  traceProgress() << "begin" << std::endl;

  if (argc >= 2) {
    // set maxStaticLocs low to test the warning
    mgr.maxStaticLocs = 1;
  }

  // test with some special files
  testFileString("first\nsecond\nthird\n");      // ordinary
  testFileString("first\nsecond\nthird no nl");  // no final newline
  testFileString("");                            // empty
  testFileString("x");                           // one char
  testFileString("\n");                          // one newline

  // test my source code
  testFile("srcloc.cc");
  testFile("srcloc.h");

  // do it again, so at least one won't be the just-added file;
  // in fact do it many times so I can see results in a profiler
  for (int i=0; i<1; i++) {
    testFile("srcloc.cc");
    testFile("srcloc.h");
  }

  traceProgress() << "end" << std::endl;

  // protect against degeneracy by printing the length of
  // the longest line
  std::cout << "\n";
  std::cout << "long line len: " << longestLen << std::endl;

  // test the statics
  std::cout << "invalid: " << toString(SL_UNKNOWN) << std::endl;
  std::cout << "here: " << toString(HERE_SOURCELOC) << std::endl;

  std::cout << "\n";
  testHashMap();
  testHashMap2();

  std::cout << "srcloc is ok\n";
}

ARGS_MAIN


#endif // TEST_SRCLOC
