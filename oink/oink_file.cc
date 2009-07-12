// see License.txt for copyright and terms of use

#include "oink_file.h"          // this module
#include "oink_util.h"
#include <dirent.h>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <iostream>

// all that will happen if this is not long enough is that the
// filename will get truncated and you will get an error when you try
// to read it
#if ! defined NAME_MAX
  #define NAME_MAX 255
#endif

void File::dump() const {
  cout << "\tfile:" << name << endl;
}

void File::appendSelfIntoList(ASTList<File> &l) const {
  l.append(new File(name)); // NOTE: clone the File since list is an owning list
}

void ProgramFile::dump() const {
  cout << "\tprogram file:" << name << endl;
}

void ProgramFile::appendSelfIntoList(ASTList<File> &fileList) const {
  int const bufsize = NAME_MAX+1; // +1 for the nul (thanks emacs)
  FILE *in = fopen(name.c_str(), "r");
  if (!in) {                    // error
    perror("file access module"); // FIX: generalize that
    throw UserError(USER_ERROR_ExitCode);
  }
  char filename[bufsize];       // filename buffer
  while(1) {
    char *result = fgets(filename, bufsize, in);
    if (!result) {              // error or EOF
      if (ferror(in)) {
        perror("file access module");      // FIX: generalize that
        throw UserError(USER_ERROR_ExitCode);
      }
      if (feof(in)) {           // done
        if (fclose(in) == 0) return; // successful close
        perror("file access module");      // FIX: generalize that
        throw UserError(USER_ERROR_ExitCode);
      }
      xassert(0);               // this "can't happen"
    }
    if (trimFilename(filename, bufsize)) fileList.append(new File(strdup(filename)));
  }
}

char *trimFilename(char *filename, int bufsize) {
  // make sure it has a nul
  filename[bufsize-1] = '\0';
  // truncate anything after (inclusive) a #
  char *pos = index(filename, '#');
  if (pos) *pos = '\0';
  // truncate anything after (inclusive) a \n
  pos = index(filename, '\n');
  if (pos) *pos = '\0';
  // trim non-alphanumerics from the front
  while (filename[0] && !isgraph(filename[0])) {
    for (int j=0; filename[j]; ++j) filename[j] = filename[j+1];
    // note that the final nul gets moved down also
  }
  // trim non-alphanumerics from the back
  {
    int j;
    for (j=0; filename[j] && isgraph(filename[j]); ++j) {}
    filename[j] = '\0';
  }
  // skip blank lines
  if (filename[0]) return filename;
  return NULL;
}

char const *getSuffix(char const *name) {
  char const *suff = getSuffix0(name);
  if (!*suff) {
    fprintf(stderr,
            "Under input lang 'suffix', all files must have a suffix: %s\n",
            name);
    throw UserError(USER_ERROR_ExitCode);
  }
  return suff;
}

