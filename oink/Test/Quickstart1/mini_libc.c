// provide annotations for part of libc; see
// oink-stack/platform-model/ for the real thing
char $tainted *getenv(char const *name);
int printf(char const $untainted *fmt, ...);
