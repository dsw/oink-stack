// ofstreamts.h

// quarl 2006-05-25 initial version, factored from 2006-05-16 astgen.cc

#ifndef OFSTREAMTS_H
#define OFSTREAMTS_H

#include <fstream.h>
#include "str.h"

// An ofstream which is timestamp-conscious.  It first writes to a temporary
// file, and on success renames it to the target destination.  However, it
// doesn't do this if the file hasn't changed.  This way we avoid messing with
// the timestamp, which is annoying since 'make' will think we have to rebuild
// everything.
//
// Another advantage of writing to a temporary file then renaming is we get
// atomic output.
//
// Call 'dontsave()' to avoid saving, e.g. if an error occurred.
class ofstreamTS : public ofstream {
  // The name of the file where the data is eventually saved
  string destFname;

  // The name of a temporary file where we write until we rename.
  //
  // This is currently "filename.tmp".  Since it is 'rename'd to destFname, it
  // must be in the same file system as destFname (i.e. generally not in
  // /tmp)
  string tmpFname;

  // Flag indicating whether the destructor should save.  See dontsave().
  bool dosave;

  // Initialize the member filenames; used internally by constructor
  const char *init_fname(string const &destFname0);

  // Open the temporary file
  void openTmp() { open(tmpFname.c_str()); }

  // Close the temporary file and rename to the destination file
  void save();

  // Close the temporary file and delete it without saving.
  void deleteTmp();

public:
  ofstreamTS(string const &destFname0) : dosave(true)
  { init_fname(destFname0); openTmp(); }
  ~ofstreamTS() { if (dosave) save(); }

  // Indicate that the output shouldn't be saved, e.g. due to an error.
  void dontsave() { dosave = false; }

  // Abort output: delete temporary file, and don't save.
  void abort() { deleteTmp(); dontsave(); }
};

#endif
