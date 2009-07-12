// see License.txt for copyright and terms of use

// $Id: archive_srz_format.h 1702 2006-04-26 10:07:32Z quarl $

#ifndef H_ARCHIVE_SRZ_FORMAT
#define H_ARCHIVE_SRZ_FORMAT

#include <map>
#include <string>
#include <iostream>
#include "exc.h"

// abstract class that provides the opt() interface for reading and writing options.
class ArchiveSrzFormat {
public:
  virtual ~ArchiveSrzFormat() {}
  virtual void opt(std::string const &key, std::string const &value) = 0;

  virtual std::string const &get(std::string const &key)
  {
    xfailure("get() not allowed in this class (144daaf9-3fa1-4e37-b138-69a927203b53)");
  }

  void opt(std::string const &key, const char* expectedValue)
  { opt(key, std::string(expectedValue)); }

  void opt(std::string const &key, bool expectedValue)
  { opt(key, ArchiveSrzFormat::boolToStr(expectedValue)); }

protected:
  static inline const char* boolToStr(bool b)
  {
    return b ? "true" : "false";
  }
};

class ArchiveSrzFormatChecker : public ArchiveSrzFormat {
public:
  ArchiveSrzFormatChecker(std::istream &in, const char* fname0) : fname(fname0) { input(in); }
  virtual ~ArchiveSrzFormatChecker() {}

  // Check if the value of KEY is equal to EXPECTEDVALUE.  Error, if not.
  void opt(std::string const &key, std::string const &expectedValue);

  // Return the value for given key.  Note that if it doesn't exist yet, it is
  // initialized to the empty string.
  std::string const &get(std::string const &key)
  { return vmap[key]; }

protected:
  void input(std::istream &in);

  const char* fname;                                // filename, for error reporting
  std::map<std::string,std::string> vmap;           // contains key/value pairs
};

class ArchiveSrzFormatWriter : public ArchiveSrzFormat {
public:
  ArchiveSrzFormatWriter(std::ostream &out0) : out(out0) {}
  virtual ~ArchiveSrzFormatWriter() {}

  // Write the given KEY/VALUE pair (to format.txt).
  void opt(std::string const &key, std::string const &value);

protected:
  std::ostream &out;
};

#endif
