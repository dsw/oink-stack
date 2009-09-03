// see License.txt for copyright and terms of use

// $Id: archive_srz.h 1703 2006-04-26 10:53:19Z quarl $

#ifndef H_ARCHIVE_SRZ
#define H_ARCHIVE_SRZ

#include "oink_util.h"                              // for UserError
#include <iostream>
#include <string>
#include <memory>

// usage:
//     ArchiveSerializerP arc = getSerializer("foo.qz");
//        or
//     ArchiveSerializerP arc = getSerializer("foo.qdir");
//
//     ostream& o1 = arc->output("file1"); o1 << "blah...";
//     ostream& o2 = arc->output("file2"); o2 << "blah...";

//     ArchiveDeserializerP arc = getDeserializer("foo.qz");
//        or
//     ArchiveDeserializerP arc = getDeserializer("foo.qdir");
//
//     istream& i1 = arc->input("file1"); i1 >> ...;
//     istream& i2 = arc->input("file2"); i2 >> ...;

// You can't output to two streams simultaneously if the output archive is a
// zip file; in this case use outputAlt() which outputs to a temporary
// ostringstream and then flushes the result later.

//     ostream& o1 = arc->outputAlt("file1"); o1 << "blah...";
//     ostream& o2 = arc->output("file2"); o2 << "blah...";
//     o1 << "...";


class ArchiveIOException {
public:
  ArchiveIOException(std::string const& error_) : error(error_) {}
  std::string const error;
};

class ArchiveSerializer {
public:
  virtual ~ArchiveSerializer();
  virtual std::ostream& output(std::string const& filename) throw(ArchiveIOException) = 0;
  virtual std::ostream& outputAlt(std::string const& filename) throw(ArchiveIOException) { return output(filename); }
  virtual const char* archiveName() const = 0;
  virtual const char* curFname() const = 0;
};

class ArchiveDeserializer {
public:
  virtual ~ArchiveDeserializer();
  virtual std::istream& input(std::string const& filename) throw(ArchiveIOException) = 0;
  virtual const char* archiveName() const = 0;
  virtual const char* curFname() const = 0;
};

typedef std::auto_ptr<ArchiveSerializer> ArchiveSerializerP;
typedef std::auto_ptr<ArchiveDeserializer> ArchiveDeserializerP;

class ArchiveSerializationManager {
public:
  ArchiveSerializationManager(const char *ext_dir, const char *ext_zip) :
    ext_dir(ext_dir), ext_zip(ext_zip)
  {}

  bool isArchiveName(std::string const& archiveName) const;

  ArchiveSerializerP getArchiveSerializer(std::string const& archiveName) const throw(ArchiveIOException, UserError);
  ArchiveDeserializerP getArchiveDeserializer(std::string const& archiveName) const throw(ArchiveIOException, UserError);

protected:
  std::string acceptableExtensions() const;

  const char *ext_dir;
  const char *ext_zip;
};

#endif // H_ARCHIVE_SRZ
