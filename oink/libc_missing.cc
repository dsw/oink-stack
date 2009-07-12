// see License.txt for copyright and terms of use

#include <stdlib.h>             // malloc
#include <string.h>             // memcpy

char *strndup0(char const *s, int const n) {
  // what is the length to copy?
  int n2 = 0;
  while (n2 < n && s[n2] != '\0') {
    ++n2;
  }

  // make a new string with room for n2 payload chars
  char *ret = (char*) malloc(n2 * sizeof(char) + 1);
  if (!ret) return NULL;

  // copy the contents
  memcpy(ret, s, n2);
  ret[n2] = '\0';
  return ret;
}
