// ofstreamts.cc

// quarl 2006-05-25 initial version, factored from 2006-05-16 astgen.cc

#include "ofstreamts.h"
#include "exc.h"

#include <stdio.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

size_t getFileSize(std::istream &i)
{
  // use 'streampos' instead of 'istream::pos_type' for gcc-2.95
  // compatibility.
  if (!i) return 0;
  std::streampos save_pos = i.tellg();
  i.seekg(0, std::ios::beg);
  std::streampos begin_pos = i.tellg();
  i.seekg(0, std::ios::end);
  std::streampos end_pos = i.tellg();
  size_t size = end_pos - begin_pos;
  i.seekg(save_pos);
  return size;
}

bool filesIdentical(const char *f1, const char *f2)
{
  std::ifstream i1(f1);
  if (!i1) return false;

  std::ifstream i2(f2);
  if (!i2) {
    xfatal(stringc << "I thought I just wrote " << f2 << ", but it doesn't exist");
    return false;
  }

  if (getFileSize(i1) != getFileSize(i2))
    return false;

  while (true) {
    if (i1.bad() || i2.bad()) {
      return false;
    }

    if (i1.eof() && i2.eof()) {
      // reached EOF and no problem.
      return true;
    }

    if (!i1 || !i2) {
      // Other error? or for some reason we reached EOF in one file but not
      // another even though file sizes are the same
      return false;
    }

    if (i1.get() != i2.get()) {
      // unidentical character
      return false;
    }
  }
}

const char *ofstreamTS::init_fname(string const &destFname0)
{
  destFname = destFname0;
  tmpFname = destFname0 & ".tmp";
  return tmpFname.c_str();
}

void ofstreamTS::save() {
  close();
  if (filesIdentical(destFname.c_str(), tmpFname.c_str())) {
    std::cout << "  file " << destFname << " unchanged, so not overwriting it.\n";
    if (unlink(tmpFname.c_str())) {
      std::cerr << "  unlink " << tmpFname << " failed\n";
    }
    return;
  }
  if (rename(tmpFname.c_str(), destFname.c_str())) {
    xfatal(stringc << "Rename " << tmpFname << " to " << destFname << " failed");
  }
}

void ofstreamTS::deleteTmp() {
  close();
  if (unlink(tmpFname.c_str())) {
    std::cerr << "  unlink " << tmpFname << " failed\n";
  }
}
