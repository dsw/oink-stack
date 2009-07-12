#include <stdio.h>
    
#ifdef CQUAL
/* to analyze */
#  define TAINTED $tainted
#  define UNTAINTED $untainted
#else
/* to compile */
#  define TAINTED
#  define UNTAINTED
#endif
    
/* Replace this with an internal home-grown compression algorithm if
   you don't believe this is within the realm of what people might
   write.  For example, the codec for I think JPEG uses Huffman
   encoding down to the bit granularity and therefor does things like
   this. */
char *launderString(char *in) {
  int len = strlen(in);
  char *out = malloc((len+1) * sizeof out[0]);
  int i, j;
  for (i=0; i<len; ++i) {
    out[i] = 0;
    for (j=0; j<8; ++j) {
      // NOTE: I can make this as bad as I want; Huffman decoding is
      // non-trivial
      if (in[i] & (1<<j)) out[i] |= (1<<j);
    }
  }
  out[i] = '\0';
  return out;
}
    
int main(int argc, char **argv) {
  int size = 1024;
  char TAINTED line[size];
  while(1) {
    char *ret = fgets(line, size, stdin);
    if (!ret) break;
    char UNTAINTED *b = launderString(line);
    /* FORMAT STRING BUG! */
    printf(b);
  }
  return 0;
}
