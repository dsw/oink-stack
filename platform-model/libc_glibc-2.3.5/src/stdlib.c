#include <glibc-fs/prelude.h>
#include <stdlib.h>

size_t __ctype_get_mb_cur_max (void) {}

double atof (const char *nptr) {}

int atoi (const char *nptr) {}

long int atol (const char *nptr) {}

long long int atoll (const char *nptr) {}

double strtod (const char $_1 * nptr, char $_1 ** endptr) {}

float strtof (const char $_1 * nptr, char $_1 ** endptr) {}

long double strtold (const char $_1 * nptr, char $_1 ** endptr) {}

long int strtol (const char $_1 * nptr, char $_1 ** endptr, int base) {}

unsigned long int strtoul (const char $_1 * nptr, char $_1 ** endptr, int base) {}

long long int strtoq (const char $_1 * nptr, char $_1 ** endptr, int base) {}

unsigned long long int strtouq (const char $_1 * nptr, char $_1 ** endptr, int base) {}

long long int strtoll (const char $_1 * nptr, char $_1 ** endptr, int base) {}
unsigned long long int strtoull (const char $_1 * nptr, char $_1 ** endptr, int base) {}

long int strtol_l (const char $_1 * nptr, char $_1 ** endptr, int base, __locale_t loc) {}

unsigned long int strtoul_l (const char $_1 * nptr, char $_1 ** endptr, int base, __locale_t loc) {}

long long int strtoll_l (const char $_1 * nptr, char $_1 ** endptr, int base, __locale_t loc) {}

unsigned long long int strtoull_l (const char $_1 * nptr, char $_1 ** endptr,
                                   int base, __locale_t loc) {}

double strtod_l (const char $_1 * nptr, char $_1 ** endptr, __locale_t loc) {}

float strtof_l (const char $_1 * nptr, char $_1 ** endptr, __locale_t loc) {}

long double strtold_l (const char $_1 * nptr, char $_1 ** endptr, __locale_t loc) {}

double __strtod_internal (const char $_1 * nptr, char $_1 ** endptr, int group) {}
float __strtof_internal (const char $_1 * nptr, char $_1 ** endptr, int group) {}
long double __strtold_internal (const char $_1 * nptr, char $_1 ** endptr, int group) {}
long int __strtol_internal (const char $_1 * nptr, char $_1 ** endptr, int base, int group) {}

unsigned long int __strtoul_internal (const char $_1 * nptr, char $_1 ** endptr, int base, int group) {}

long long int __strtoll_internal (const char $_1 * nptr, char $_1 ** endptr, int base, int group) {}

unsigned long long int __strtoull_internal (const char $_1 * nptr, char $_1 ** endptr, int base, int group) {}

// theoretically we could polymorphically use taintedness of N, but we don't
// really track taint of integers
char $tainted *l64a (long int n) {}

long int a64l (const char *s) {}

long int random (void) {}

void srandom (unsigned int seed) {}

char $tainted *initstate (unsigned int seed, char $tainted *statebuf, size_t statelen) {}

char $tainted *setstate (char $tainted *statebuf) {}

// struct random_data
// {
//     int32_t *fptr;
//     int32_t *rptr;
//     int32_t *state;
//     int rand_type;
//     int rand_deg;
//     int rand_sep;
//     int32_t *end_ptr;
// };

int random_r (struct random_data * buf, int32_t * result) {}

int srandom_r (unsigned int seed, struct random_data *buf) {}

int initstate_r (unsigned int seed, char $tainted * statebuf, size_t statelen, struct random_data * buf) {}

int setstate_r (char $tainted * statebuf, struct random_data * buf) {}

int rand (void) {}

void srand (unsigned int seed) {}

int rand_r (unsigned int *seed) {}

double drand48 (void) {}
double erand48 (unsigned short int xsubi[3]) {}

long int lrand48 (void) {}
long int nrand48 (unsigned short int xsubi[3]) {}

long int mrand48 (void) {}
long int jrand48 (unsigned short int xsubi[3]) {}

void srand48 (long int seedval) {}
unsigned short int *seed48 (unsigned short int seed16v[3]) {}
void lcong48 (unsigned short int param[7]) {}

// struct drand48_data
// {
//     unsigned short int x[3];
//     unsigned short int old_x[3];
//     unsigned short int c;
//     unsigned short int init;
//     unsigned long long int a;
// };

int drand48_r (struct drand48_data * buffer, double * result) {}
int erand48_r (unsigned short int xsubi[3], struct drand48_data * buffer, double * result) {}

int lrand48_r (struct drand48_data * buffer, long int * result) {}
int nrand48_r (unsigned short int xsubi[3], struct drand48_data * buffer, long int * result) {}

int mrand48_r (struct drand48_data * buffer, long int * result) {}
int jrand48_r (unsigned short int xsubi[3], struct drand48_data * buffer, long int * result) {}

int srand48_r (long int seedval, struct drand48_data *buffer) {}

int seed48_r (unsigned short int seed16v[3], struct drand48_data *buffer) {}

int lcong48_r (unsigned short int param[7], struct drand48_data *buffer) {}

//// These are in malloc.c
// void *malloc (size_t size) {}
// void *calloc (size_t nmemb, size_t size) {}
// void *realloc (void *ptr, size_t size) {}
// void free (void *ptr) {}
// void cfree (void *ptr) {}
// void *valloc (size_t size) {}

int posix_memalign (void **memptr, size_t alignment, size_t size) {}

FAKEBUILTIN
void abort (void) { __void_noreturn(); }

int atexit (void (*__func) (void)) {}

int on_exit (void (*__func) (int status, void *arg), void *arg) {}

FAKEBUILTIN
void exit (int status) { __void_noreturn(); }

FAKEBUILTIN
void _Exit (int status) { __void_noreturn(); }

char $tainted *getenv (const char *name) {}

char $tainted *__secure_getenv (const char *name) {}

int putenv (char *string) {}

int setenv (const char *name, const char *value, int replace) {}

int unsetenv (const char *name) {}

int clearenv (void) {}

// these should really be const char* inputs
char $tainted *mktemp (char *template) {}

int mkstemp (char *template) {}

int mkstemp64 (char *template) {}

char $tainted *mkdtemp (char *template) {}

int system (const char *command) {}

char $tainted *canonicalize_file_name (const char *name) {}

char $tainted *realpath (const char * name, char $tainted * resolved) {}

// typedef int (*__compar_fn_t) (const void *, const void *);

void *bsearch (const void *key, const void *base,
               size_t nmemb, size_t size, __compar_fn_t compar) {}

void qsort (void *base, size_t nmemb, size_t size, __compar_fn_t compar) {}

FAKEBUILTIN
int abs (int x) {}

FAKEBUILTIN
long int labs (long int x) {}

FAKEBUILTIN
long long int llabs (long long int x) {}

div_t div (int numer, int denom) {}
ldiv_t ldiv (long int numer, long int denom) {}

lldiv_t lldiv (long long int numer, long long int denom) {}

char $tainted *ecvt (double value, int ndigit, int * decpt, int * sign) {}

char $tainted *fcvt (double value, int ndigit, int * decpt, int * sign) {}

char $tainted *gcvt (double value, int ndigit, char $tainted *buf) {}

char $tainted *qecvt (long double value, int ndigit, int * decpt, int * sign) {}
char $tainted *qfcvt (long double value, int ndigit, int * decpt, int * sign) {}
char $tainted *qgcvt (long double value, int ndigit, char $tainted *buf) {}

int ecvt_r (double value, int ndigit, int * decpt, int * sign, char $tainted * buf, size_t len) {}
int fcvt_r (double value, int ndigit, int * decpt, int * sign, char $tainted * buf, size_t len) {}

int qecvt_r (long double value, int ndigit, int * decpt, int * sign, char $tainted * buf, size_t len) {}
int qfcvt_r (long double value, int ndigit, int * decpt, int * sign, char $tainted * buf, size_t len) {}

int mblen (const char *s, size_t n) {}

int mbtowc (wchar_t $_1_2 * pwc, const char $_1 * s, size_t n) {}

int wctomb (char $_1_2 *s, wchar_t $_1 wchar) {}

size_t mbstowcs (wchar_t $_1_2 * pwcs, const char $_1 * s, size_t n) {}

size_t wcstombs (char $_1_2 * s, const wchar_t $_1 * pwcs, size_t n) {}

int rpmatch (const char *response) {}

// TODO: I don't know what this does so tainted all for now
int getsubopt (char $tainted ** optionp, char $tainted *const * __tokens,
               char $tainted ** valuep) {}

//// in crypt.c
// void setkey (const char *key) {}

int posix_openpt (int oflag) {}

int grantpt (int fd) {}

int unlockpt (int fd) {}

char $tainted *ptsname (int fd) {}

int ptsname_r (int fd, char $tainted *buf, size_t buflen) {}

int getpt (void) {}

int getloadavg (double loadavg[], int nelem) {}
