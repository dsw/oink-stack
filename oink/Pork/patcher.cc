// See License.txt for copyright and terms of use

#include "patcher.h"
#include "squash_util.h"

#include <fstream>
#include <ios>
#include <sstream>
#include <map>

// **** UnboxedLoc

StringRef UnboxedLoc::set(SourceLoc loc) {
  StringRef file(NULL);
  int line(0);
  int col(0);
  sourceLocManager->decodeLineCol(loc, file, line, col);
  this->line = line;
  this->col = col;
  return file;
}

SourceLoc UnboxedLoc::toSourceLoc(StringRef file) {
  return sourceLocManager->encodeLineCol(file, line, col);
}

// **** UnboxedPairLoc

UnboxedPairLoc::UnboxedPairLoc(PairLoc const &pairLoc) {
  xassert(pairLoc.hasExactPosition());
  this->file = first.set(pairLoc.first.loc());

  if (pairLoc.first.loc() == pairLoc.second.loc()) {
    second = first;
    return;
  }
  StringRef file = second.set(pairLoc.second.loc());
  xassert(this->file == file);
}

UnboxedPairLoc::UnboxedPairLoc(StringRef file, UnboxedLoc const &from,
                               UnboxedLoc const &to)
  : std::pair<UnboxedLoc, UnboxedLoc>(from, to)
{
  this->file = file;
}

std::string UnboxedPairLoc::toString() const {
  std::stringstream ss;
  ss << file << "(" << first.line << ":" << first.col << "-"
     << second.line << ":" << second.col << ")";
  return ss.str();
}

// **** Patcher

Patcher::Patcher(std::ostream& out, bool recursive)
  : out(out), recursive(recursive) {   
}

Patcher::~Patcher() {
  // dsw: it is a problem having this here; do it manually
//   flush();
}

// This method goes through the pains of producing correct patch
// output even in cases of multiple continuous things being modified
// on the same line
void Patcher::flushQueue(hunk_queue &q) {
  patch_map::value_type const *first = *q.begin();
  patch_map::value_type const *last = *q.rbegin();
  
  load(first->first.file);
  // get ready for first line of output
  unsigned int offset = 0;
  unsigned int prev_line = first->first.first.line;
  std::string line = lines[prev_line - 1];
  std::stringstream ss;
  unsigned extraLines = 1;
  ss << '+';
  for(hunk_queue::const_iterator it = q.begin(); 
      it != q.end();
      ++it) {
    UnboxedPairLoc const &loc = (*it)->first;
    std::string const &str = (*it)->second;

    if (loc.first.line != prev_line) {
      // consecutive UnboxedPairLocs can not be queued
      // if they are separated by more than 1 newline
      xassert(prev_line + 1 == loc.first.line);
      ss << line.substr(offset) << '\n';
      ss << '+';
      prev_line = loc.first.line;
      line = lines[prev_line - 1];
      offset = 0;
    }
    ss << line.substr(offset, loc.first.col - 1 - offset);
    if (loc.first.line != loc.second.line) {
      prev_line = loc.second.line;
      line = lines[prev_line - 1];
      // if replacing a multi-line chunk
      // the replacement may contain a different
      // number of lines, so make no assumptions
      extraLines -= loc.second.line - loc.first.line;
    }
    offset = loc.second.col - 1;

    // tokenize replacement string by \n and insert \n+ for diff
    std::string::size_type last_n = 0;
    for (std::string::size_type n = str.find('\n');
         n != std::string::npos;
         last_n = n + 1, n = str.find('\n', last_n))
      {
        std::string substr = str.substr(last_n, n - last_n);
        ss << substr << "\n+";
        extraLines++;
      }
    ss << str.substr(last_n);
  }
  // output remainder of stuff
  ss << line.substr(offset) << '\n';
  printHunkHeaderAndDeletedLines
    (first->first.first.line,
     last->first.second.line,
     last->first.second.line - first->first.first.line + extraLines, 
     first->first.file,
     out);
  out << ss.str();
  q.clear();
}
 
void Patcher::flush() {
  std::string last_file;
  // queue representing lines to merge & produce hunks
  hunk_queue q; 
  for (patch_map::const_iterator hunks = output.begin();
       hunks != output.end();
       hunks++) {
    UnboxedPairLoc const &range = hunks->first;
    if (last_file != hunks->first.file) {
      if (!q.empty()) flushQueue(q);
      last_file = hunks->first.file;
      std::string real_file = resolveAbsolutePath(last_file);
      out << "--- " << real_file << "\n";
      out << "+++ " << real_file << "\n";
      // flush the queue if this chunk isn't on the same line as last chunk
    } else if (!q.empty()
               && range.first.line - (*q.rbegin())->first.second.line > 1) {
      flushQueue(q);
    }
    // save pointer to element pointed by iterator
    q.push_back(&(*hunks));
  }
  if (!q.empty()) flushQueue(q);
}

void Patcher::load(std::string const &inFile) {
  std::string const file = resolveAbsolutePath(inFile);
  if (this->file == file)
    return;

  this->file = file;
  lines.clear();

  std::ifstream f(file.c_str());
  if (!f.is_open())
    {
      std::cerr << "Could not open " << file << std::endl;
      exit(1);
    }
  std::string line;
  while (getline(f, line)) {
    lines.push_back(line);
  }
  f.close();
}

void Patcher::copy(unsigned int minLine, unsigned int maxLine,
                   std::string const &file,
                   std::ostream &ostream, std::string const &prefix)
{
  load(file);
  xassert(maxLine <= lines.size());
  for (unsigned int i = minLine;i <= maxLine;i++) {
    ostream << prefix << lines[i-1] << "\n";
  }
}

void Patcher::printHunkHeaderAndDeletedLines
(unsigned int minLine, unsigned int maxLine,
 int added_lines,
 std::string const &file,
 std::ostream &ostream)
{
  ostream << "@@"
          << " -" << minLine << "," << (maxLine - minLine + 1)
          << " +" << minLine << "," << added_lines
          << " @@\n";

  copy(minLine, maxLine, file, ostream, "-");
}

std::string Patcher::getLine(unsigned int line, std::string const &file) {
  load(file);
  xassert(line <= lines.size());
  return lines[line - 1];
}

std::string Patcher::getRange(UnboxedPairLoc const &loc) {
  unsigned int minLine = loc.first.line;
  unsigned int minCol = loc.first.col;
  unsigned int maxLine = loc.second.line;
  unsigned int maxCol = loc.second.col;
  std::string const &file = loc.file;
  xassert(minLine < maxLine || (minLine == maxLine && minCol < maxCol));

  load(file);
  
  std::stringstream ss;
  xassert(maxLine <= lines.size());

  //ff the iterator to be inline with loc
  patch_map::iterator it = output.begin();
  for (;it != output.end() 
         && (it->first.file != file || it->first.first.line < minLine); ++it);

  // Indicates displacement from loc.second.col caused by last line 
  // being affected by previous patches; Can be negative
  int increasedOffset = 0;

  for (unsigned int i = minLine;i <= maxLine;i++) {
    std::string::size_type offset = 0;
    // recursive bit, merge in patches produced by printPatch
    // into what's returned
    for (;it != output.end() 
           && recursive
           && it->first.first.line == i
           && it->first.file == file;
         ++it) {
      // it doesn't make sense to cut out part of code that has been
      // patched already
      xassert(it->first.second.line < maxLine
              || (it->first.second.line == maxLine 
                  && it->first.second.col <= maxCol));
      // first provide actual file content
      ss << lines[i-1].substr(offset, it->first.first.col - 1 - offset)
         << it->second;
      i = it->first.second.line;
      if (i == maxLine) {
        // length of the last output line
        std::string::size_type len = it->second.size();
        std::string::size_type pos = it->second.rfind("\n");
        if (pos != std::string::npos) {
          len -= pos + 1;
        }
        increasedOffset += it->first.first.line == it->first.second.line 
          ? it->first.first.col - 1
          : 0;
        increasedOffset += len - (it->first.second.col - 1);
      }
      offset = it->first.second.col - 1;
    }
    // the rest is actual file content
    ss << lines[i-1].substr(offset) << "\n";
  }

  std::string str = ss.str();
  std::string::size_type postLastNewline = 0;
  if (minLine != maxLine) {
    if (!str.empty() && str[str.size() - 1] == '\n') {
      str.erase(str.size() - 1);
    }
    postLastNewline = str.rfind('\n');
    xassert(postLastNewline != std::string::npos);
    postLastNewline++;
  }
  // elsa indexes from 1
  minCol--;
  maxCol--;
  // erase the end first to avoid messing with offsets
  if (str.size() > postLastNewline + maxCol) {
    str.erase(postLastNewline + maxCol + increasedOffset);
  }
  str.erase(0, minCol);

  return str;
}

// One is allowed replace an existing patch, but can't have ones that
// intersect
void Patcher::printPatch(std::string const &str, UnboxedPairLoc const &loc,
                         bool recursive)
{
  std::string file(loc.file);
  unsigned int minLine = loc.first.line;
  unsigned int minCol = loc.first.col;
  unsigned int maxLine = loc.second.line;
  unsigned int maxCol = loc.second.col;
  xassert(minLine < maxLine || (minLine == maxLine && minCol < maxCol));

  // check that no existing entries conflict
  for (patch_map::iterator it = output.begin();
       it != output.end();
       ++it) {
    UnboxedPairLoc const &other = it->first;
    unsigned int otherMinLine = other.first.line;
    unsigned int otherMinCol = other.first.col;
    unsigned int otherMaxLine = other.second.line;
    unsigned int otherMaxCol = other.second.col;
  
    if (other.file != file
        || otherMaxLine < minLine 
        || (otherMaxLine == minLine
            && otherMaxCol <= minCol)) {
      continue;
    }
    // other.file == file by above
    if (otherMinLine > maxLine
        || (otherMinLine == maxLine
            && otherMinCol >= maxCol)) break;
    // seems that the patches intersect
    bool sameLoc = minLine == otherMinLine && minCol == otherMinCol
      && maxLine == otherMaxLine && maxCol == otherMaxCol;
    const bool collision = !(recursive || sameLoc);
    if (collision) {
      std::cerr << file << ':' << minLine << ':' << minCol
                << ": Incompatible edits of the same code" << std::endl;
      xassert(!collision);
    }
    if (recursive) {
      // assert that other is within loc
      xassert((minLine < otherMinLine 
               || (minLine == otherMinLine && minCol <= otherMinCol))
              && (maxLine > otherMaxLine 
                  || (maxLine == otherMaxLine && maxCol >= otherMaxCol)));
      // then delete other
      output.erase(it);
      // afaik there is no way to erase from a map and continue
      // i suppose i could store these in a vector and delete them after
      // i iterate the map, but i don't much care for efficiency in this case
      it = output.begin();
      continue;
    }
  }

  output[loc] = str;
}

void Patcher::insertBefore(char const *file, UnboxedLoc const &loc,
                           std::string const &str) {
  output[UnboxedPairLoc (file, loc, loc)] = str;
}

void Patcher::insertBefore
(CPPSourceLoc const &csl, std::string const &str, long offset)
{
  UnboxedLoc loc;
  char const *file = loc.set(csl.loc());
  loc.col += offset;
  insertBefore(file, loc, str); 
}

unsigned int Patcher::lineCount(std::string const &file) {
  load(file);
  return lines.size();
}

std::string Patcher::resolveAbsolutePath(std::string const &inPath) const {
  std::string path = ::resolveAbsolutePath(dir, inPath);
  return path;
}

void Patcher::setFile(std::string const &file) {
  dir = "";
  if (file.empty()) return;
  std::string::size_type pos = file.rfind('/');
  if (pos != std::string::npos) {
    dir = file.substr(0, pos + 1);
  }
}
