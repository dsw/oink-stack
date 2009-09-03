// see License.txt for copyright and terms of use

#include "archive_srz.h"
#include "oink_file.h"
#include "oink_util.h"

ArchiveSerializationManager *archiveSrzManager = new ArchiveSerializationManager(".xdir", ".xz");

void testrw(std::string const& archivename)
{
  {
    ArchiveSerializerP a = archiveSrzManager->getArchiveSerializer(archivename);
    std::cout << "opened ArchiveSerializer '" << a->archiveName() << "'" << std::endl;

    a->output("file1") << "this is file1";
    std::cout << "  wrote to '" << a->curFname() << "'" << std::endl;

    a->output("file2") << "this is file2";
    std::cout << "  wrote to '" << a->curFname() << "'" << std::endl;
  }

  {
    ArchiveDeserializerP a = archiveSrzManager->getArchiveDeserializer(archivename);
    std::cout << "opened ArchiveDeserializer " << a->archiveName() << "'" << std::endl;

    std::string s;

    std::getline(a->input("file1"), s);
    std::cout << "  read from '" << a->curFname() << "': " << s << std::endl;

    std::getline(a->input("file2"), s);
    std::cout << "  read from '" << a->curFname() << "': " << s << std::endl;

    try {
      std::cout << "  read from bad file 'file3': ";
      a->input("file3");
      std::cout << "FAILED, did not throw ArchiveIOException\n";
    } catch(ArchiveIOException) {
      std::cout << "good, threw ArchiveIOException\n";
    }
  }
}

void testnonexistant(std::string const& archivename)
{
  try {
    std::cout << "test opening invalid archive " << archivename << ": ";

    archiveSrzManager->getArchiveDeserializer(archivename);
    std::cout << "FAILED, did not throw ArchiveIOException\n";
  } catch(UserError &) {
    std::cout << "good, threw UserError\n";
  } catch(ArchiveIOException &) {
    std::cout << "good, threw ArchiveIOException\n";
  }
}

int main()
{
  try {
    testrw("foo1.xdir");
    testrw("foo1.xz");

    testnonexistant("badfilename.xyz");
    testnonexistant("/etc/xxx.qz");
  } catch (xBase &e) {
    std::cerr << "EXCEPTION: " << e << std::endl;
  } catch (...) {
    std::cerr << "UNKNOWN EXCEPTION\n";
  }
}
