// see License.txt for copyright and terms of use

// $Id: archive_srz.cc 2317 2006-07-02 06:31:52Z quarl $

#include "archive_srz.h"

#include <memory>
#include <vector>
#include <sstream>

#include "oink_file.h"
#include "oink_util.h"

#include <sys/types.h>
#include <sys/stat.h>

ArchiveSerializer::~ArchiveSerializer() {}

ArchiveDeserializer::~ArchiveDeserializer() {}

#if !defined (ARCHIVE_SRZ_DIR) && !defined (ARCHIVE_SRZ_ZIP)
# error "must have at least one of ARCHIVE_SRZ_DIR or ARCHIVE_SRZ_ZIP"
#endif


#ifdef ARCHIVE_SRZ_DIR

// implementation of ArchiveSerializer/ArchiveDeserializer which uses a
// directory

#include <fcntl.h>
#include <fstream>

static inline bool isdir(const char* path)
{
  struct stat sb;
  return (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode));
}

template <class T> class auto_ptr_vector {
public:
  typedef std::vector<T*> vector_t;
  auto_ptr_vector()
  // : t(0)
  {}
  ~auto_ptr_vector() {
    for (typename vector_t::iterator j = v.begin(); j != v.end(); ++j)
    {
      delete *j;
    }
    // delete t;
  }
  void push(T* x) {
    v.push_back(x);
    // delete t;
    // t = x;
  }

protected:
  vector_t v;
  // T* t;
};

#define ARCHIVE_IO_EXCEPTION( MSG )                               \
do {                                                              \
  std::ostringstream o;                                           \
  o MSG;                                                          \
  throw ArchiveIOException(o.str());                              \
} while(0);

class DirectoryArchiveSerializer : public ArchiveSerializer {
public:
  DirectoryArchiveSerializer(std::string const& dirname_) throw(ArchiveIOException)
  : dirname(dirname_)
  {
    mkdir(dirname.c_str(), S_IRWXU);
    if (!isdir(dirname.c_str())) {
      ARCHIVE_IO_EXCEPTION(<< "Failed to create directory for archive '" << dirname << "'");
    }
  }
  virtual ~DirectoryArchiveSerializer() {}

  std::ostream& output(std::string const& filename) throw(ArchiveIOException)
  {
    fn = dirname + '/' + filename;
    std::ofstream* f = new std::ofstream(fn.c_str());
    fv.push(f);
    if (!*f) {
      ARCHIVE_IO_EXCEPTION(<< "Failed to open file '" << fn << "' for writing");
    }
    return *f;
  }

  const char * archiveName() const { return dirname.c_str(); }
  const char * curFname() const { return fn.c_str(); }

protected:
  std::string dirname;
  auto_ptr_vector<std::ofstream> fv;
  std::string fn;
};

class DirectoryArchiveDeserializer : public ArchiveDeserializer {
public:
  DirectoryArchiveDeserializer(std::string const& dirname_) throw(ArchiveIOException)
    : dirname(dirname_)
  {
    if (!isdir(dirname.c_str())) {
      ARCHIVE_IO_EXCEPTION(
        << "Failed to open archive '" << dirname << "' - no such directory");
    }
  }

  std::istream& input(std::string const& filename) throw(ArchiveIOException)
  {
    fn = dirname + '/' + filename;
    std::ifstream* f = new std::ifstream(fn.c_str());
    fv.push(f);
    if (!*f) {
      ARCHIVE_IO_EXCEPTION( << "Failed to open file '" << fn << "' for reading");
    }
    return *f;
  }

  const char * archiveName() const { return dirname.c_str(); }
  const char * curFname() const { return fn.c_str(); }

protected:
  std::string dirname;
  auto_ptr_vector<std::ifstream> fv;
  std::string fn;
};
#endif // ARCHIVE_SRZ_DIR

#ifdef ARCHIVE_SRZ_ZIP

// implementation of ArchiveSerializer/ArchiveDeserializer which uses a
// zip file.  requires zipios++.

#include <zipios++/zipoutputstream.h>
#include <zipios++/zipfile.h>

class ZipArchiveSerializer : public ArchiveSerializer {
public:
  ZipArchiveSerializer(std::string const& zipname_) throw(ArchiveIOException)
    : zipname(zipname_), zos(zipname.c_str())
  {
    if (!zos) {
      ARCHIVE_IO_EXCEPTION(<< "Failed to open zip archive '" << zipname << "' for writing");
    }
  }

  ~ZipArchiveSerializer() { flushAltStreams(); }

  std::ostream& output(std::string const& filename) throw(ArchiveIOException)
  {
    fn = std::string("(") + zipname + ')' + filename;
    zos.putNextEntry(filename.c_str());
    if (!zos) {
      ARCHIVE_IO_EXCEPTION(<< "Failed to write to zip member '" << filename << "'");
    }

    return zos;
  }

  std::ostream& outputAlt(std::string const& filename) throw(ArchiveIOException)
  {
    std::ostringstream * str = new std::ostringstream;
    altstreams.push_back(make_pair(filename, str));
    return *str;
  }

  void flushAltStreams() {
    for (altstream_map_t::iterator i = altstreams.begin(); i != altstreams.end(); ++i)
    {
      this->output(i->first) << i->second->str();
      delete i->second;
    }
    altstreams.clear();
  }

  const char * archiveName() const { return zipname.c_str(); }
  const char * curFname() const { return fn.c_str(); }

protected:
  std::string zipname;
  zipios::ZipOutputStream zos;
  std::string fn;
  typedef std::vector<std::pair<std::string, std::ostringstream *> > altstream_map_t;
  altstream_map_t altstreams;
};

class ZipArchiveDeserializer : public ArchiveDeserializer {
public:
  ZipArchiveDeserializer(std::string const& zipname_) throw(ArchiveIOException)
    : zipname(zipname_) //, zf(zipname.c_str())
  {
    // First check if file exists and is non empty.  This isn't strictly
    // necessary as ZipFile() will throw an exception, but it improves error
    // messages.
    struct stat sb;
    if (stat(zipname.c_str(), &sb)) {
      ARCHIVE_IO_EXCEPTION(<< "File '" << zipname << "' does not exist");
    }
    if (sb.st_size == 0) {
      ARCHIVE_IO_EXCEPTION(<< "File '" << zipname << "' is a empty (corrupt zipfile)");
    }

    try {
      zf = zipios::ZipFile(zipname.c_str());
    } catch(zipios::FCollException &e) {
      ARCHIVE_IO_EXCEPTION(<< "Failed to open zip archive '" << zipname << "' for reading: " << e.what());
    }

    if (!zf.isValid()) {
      ARCHIVE_IO_EXCEPTION(<< "Failed to open zip archive '" << zipname << "' for reading");
    }
  }

  // virtual ~ZipArchiveDeserializer() {}

  std::istream& input(std::string const& filename) throw(ArchiveIOException)
  {
    fn = std::string("(") + zipname + ')' + filename;
    std::istream* f;
    try {
      f = zf.getInputStream(filename);
    } catch(zipios::InvalidStateException & e) {
      ARCHIVE_IO_EXCEPTION(<< "Failed to get zip member '" << filename << "': " << e.what());
    }
    fv.push(f);
    if (!f || !*f) {
      ARCHIVE_IO_EXCEPTION(<< "Failed to get zip member '" << filename << "'");
    }
    return *f;
  }

  const char * archiveName() const { return zipname.c_str(); }
  const char * curFname() const { return fn.c_str(); }

protected:
  std::string zipname;
  zipios::ZipFile zf;
  auto_ptr_vector<std::istream> fv;
  std::string fn;
};

#endif // ARCHIVE_SRZ_ZIP

std::string ArchiveSerializationManager::acceptableExtensions() const
{
  std::string r;

#ifdef ARCHIVE_SRZ_DIR
  r += " '";
  r += ext_dir;
  r += "'";
#endif

#ifdef ARCHIVE_SRZ_ZIP
  r += " '";
  r += ext_zip;
  r += "'";
#endif

  return r;
}

bool ArchiveSerializationManager::isArchiveName(std::string const &archiveName) const
{
  const char* suffix = getSuffix0(archiveName.c_str());

  if (streq(ext_dir, suffix)) {                     // qdir/odir
#ifdef ARCHIVE_SRZ_DIR
    return true;
#else
    userFatalError(SL_UNKNOWN,
                   "Sorry, extension '%s' not supported -- recompile with ARCHIVE_SRZ_DIR",
                   ext_dir);
#endif
  }

  if (streq(ext_zip, suffix)) {                     // qz/oz
#ifdef ARCHIVE_SRZ_ZIP
    return true;
#else
    userFatalError(SL_UNKNOWN,
                   "Sorry, extension '%s' not supported -- recompile with ARCHIVE_SRZ_ZIP",
                   ext_zip);
#endif
  }

  return false;
}

ArchiveSerializerP ArchiveSerializationManager::getArchiveSerializer(std::string const &archiveName) const
    throw(ArchiveIOException, UserError)
{
  const char* suffix = getSuffix0(archiveName.c_str());

  if (streq(ext_dir, suffix)) {                     // qdir/odir
#ifdef ARCHIVE_SRZ_DIR
    return ArchiveSerializerP(new DirectoryArchiveSerializer(archiveName));
#else
  userFatalError(SL_UNKNOWN,
                 "Invalid archive '%s': must end in one of these suffixes: %s.  "
                 "Please re-compile with you ARCHIVE_SRZ_DIR to support %s files.",
                 archiveName.c_str(), acceptableExtensions().c_str(), ext_dir);
#endif
  }

  if (streq(ext_zip, suffix)) {                     // qz/oz
#ifdef ARCHIVE_SRZ_ZIP
    return ArchiveSerializerP(new ZipArchiveSerializer(archiveName));
#else
  userFatalError(SL_UNKNOWN,
                 "Invalid archive '%s': must end in one of these suffixes: %s.  "
                 "Please re-compile with you ARCHIVE_SRZ_ZIP to support %s files.",
                 archiveName.c_str(), acceptableExtensions().c_str(), ext_zip);
#endif
  }

  userFatalError(SL_UNKNOWN,
                 "Invalid archive '%s': must end in one of these suffixes: %s",
                 archiveName.c_str(), acceptableExtensions().c_str());
}

ArchiveDeserializerP ArchiveSerializationManager::getArchiveDeserializer(std::string const& archiveName) const
    throw(ArchiveIOException, UserError)
{
  const char* suffix = getSuffix0(archiveName.c_str());

#ifdef ARCHIVE_SRZ_DIR
  if (streq(ext_dir, suffix)) {
    return ArchiveDeserializerP(new DirectoryArchiveDeserializer(archiveName));
  }
#endif

#ifdef ARCHIVE_SRZ_ZIP
  if (streq(ext_zip, suffix)) {
    return ArchiveDeserializerP(new ZipArchiveDeserializer(archiveName));
  }
#endif

  userFatalError(SL_UNKNOWN, "Invalid archive '%s': must have one of these suffixes: %s",
                 archiveName.c_str(), acceptableExtensions().c_str());
}
