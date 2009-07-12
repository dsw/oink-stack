// strtokpc.h

// quarl 2006-05-24 initial version

// String tokenizing class.  Modifies an input string in-place to
// null-terminate individual tokens, and undoes modifications on destruction.
// It does not allocate or copy strings, so suitable to be used in an inner
// loop.

// It only supports iterating linearly through the tokens (its functionality
// is similar to strtok_r, but it undoes modifications).

#ifndef STRTOKPC_H
#define STRTOKPC_H

class StrtokParseC {
  char *str;
  char save;

public:
  StrtokParseC(char const *str0) : str(const_cast<char*>(str0)), save('\0') {}
  ~StrtokParseC() { if (save != '\0') { *str = save; } }

  // Return a token, or NULL.  The token is only valid until the next call to
  // nextToken() or until destruction.
  char const *nextToken(char const delim) {
    if (save != '\0') {
      *str = save;
      ++str;
      save = '\0';
    }

    if (*str == '\0') return NULL;

    char const *tok = str;

    while (1) {
      if (*str == '\0') {
        save = '\0';
        return tok;
      }

      if (*str == delim) {
        save = delim;
        *str = '\0';
        return tok;
      }

      ++str;
    }
  }
};

#endif

