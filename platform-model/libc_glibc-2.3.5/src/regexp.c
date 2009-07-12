
// globals written by step() and advance() and accessed by application programmer
char *loc1;
char *loc2;
char *locs;

// this function is in the header file, to use macros defined by user:
//   char * compile (char * instring, char * expbuf, const char * endbuf, int eof)

int step (const char * string, const char * expbuf) {
    locs[0] = loc1[0] = loc2[0] = string[0];
    locs[0] = loc1[0] = loc2[0] = expbuf[0];
}

int advance (const char * string, const char * expbuf) {
    locs[0] = loc1[0] = loc2[0] = string[0];
    locs[0] = loc1[0] = loc2[0] = expbuf[0];
}
