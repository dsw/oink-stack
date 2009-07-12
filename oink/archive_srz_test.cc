// see License.txt for copyright and terms of use

#include "archive_srz.h"
#include "oink_file.h"
#include "oink_util.h"

ArchiveSerializationManager *archiveSrzManager = new ArchiveSerializationManager(".xdir", ".xz");

void testrw(std::string const& archivename)
{
  {
    ArchiveSerializerP a = archiveSrzManager->getArchiveSerializer(archivename);
    cout << "opened ArchiveSerializer '" << a->archiveName() << "'" << endl;

    a->output("file1") << "this is file1";
    cout << "  wrote to '" << a->curFname() << "'" << endl;

    a->output("file2") << "this is file2";
    cout << "  wrote to '" << a->curFname() << "'" << endl;
  }

  {
    ArchiveDeserializerP a = archiveSrzManager->getArchiveDeserializer(archivename);
    cout << "opened ArchiveDeserializer " << a->archiveName() << "'" << endl;

    std::string s;

    std::getline(a->input("file1"), s);
    cout << "  read from '" << a->curFname() << "': " << s << endl;

    std::getline(a->input("file2"), s);
    cout << "  read from '" << a->curFname() << "': " << s << endl;

    try {
      cout << "  read from bad file 'file3': ";
      a->input("file3");
      cout << "FAILED, did not throw ArchiveIOException\n";
    } catch(ArchiveIOException) {
      cout << "good, threw ArchiveIOException\n";
    }
  }
}

void testnonexistant(std::string const& archivename)
{
  try {
    cout << "test opening invalid archive " << archivename << ": ";

    archiveSrzManager->getArchiveDeserializer(archivename);
    cout << "FAILED, did not throw ArchiveIOException\n";
  } catch(UserError &) {
    cout << "good, threw UserError\n";
  } catch(ArchiveIOException &) {
    cout << "good, threw ArchiveIOException\n";
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
    cerr << "EXCEPTION: " << e << endl;
  } catch (...) {
    cerr << "UNKNOWN EXCEPTION\n";
  }
}
