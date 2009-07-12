// $Id$

// this file combines annotations for stdio.h and libio.h

#include <glibc-fs/prelude.h>
#define attribute_hidden /*nothing*/
#define _IO_FILE_plus _IO_FILE
#include <stdio.h>

// struct _IO_wide_data
// {
//     wchar_t *_IO_read_ptr;
//     wchar_t *_IO_read_end;
//     wchar_t *_IO_read_base;
//     wchar_t *_IO_write_base;
//     wchar_t *_IO_write_ptr;
//     wchar_t *_IO_write_end;
//     wchar_t *_IO_buf_base;
//     wchar_t *_IO_buf_end;

//     wchar_t *_IO_save_base;
//     wchar_t *_IO_backup_base;
//     wchar_t *_IO_save_end;

//     __mbstate_t _IO_state;
//     __mbstate_t _IO_last_state;
//     struct _IO_codecvt _codecvt;

//     wchar_t _shortbuf[1];

//     const struct _IO_jump_t *_wide_vtable;
// };

// struct _IO_FILE {
//     int _flags;

//     char* _IO_read_ptr;
//     char* _IO_read_end;
//     char* _IO_read_base;
//     char* _IO_write_base;
//     char* _IO_write_ptr;
//     char* _IO_write_end;
//     char* _IO_buf_base;
//     char* _IO_buf_end;

//     char *_IO_save_base;
//     char *_IO_backup_base;
//     char *_IO_save_end;

//     struct _IO_marker *_markers;

//     struct _IO_FILE *_chain;

//     int _fileno;
//     int _flags2;
//     _IO_off_t _old_offset;

//     unsigned short _cur_column;
//     signed char _vtable_offset;
//     char _shortbuf[1];

//     _IO_lock_t *_lock;

//     _IO_off64_t _offset;

//     struct _IO_codecvt *_codecvt;
//     struct _IO_wide_data *_wide_data;
//     int _mode;

//     char _unused2[15 * sizeof (int) - 2 * sizeof (void *)];
// };

// struct _IO_marker {
//     struct _IO_marker *_next;
//     struct _IO_FILE *_sbuf;

//     int _pos;
// };

// Many of the simple stdio functions such as getc are actually inlined to
// direct FILE* data structure accesses (see libio.h, bits/stdio.h).
//
//  Thus we return a "tainted" FILE*.  stdin, et al also need to be tainted
//  globally.

// not inline because it's needed in other files.
void __taint_FILE(struct _IO_FILE* p)
{
    __DO_TAINT(p->_IO_read_ptr);
    __DO_TAINT(p->_IO_read_end);
    __DO_TAINT(p->_IO_read_base);
    __DO_TAINT(p->_IO_write_base);
    __DO_TAINT(p->_IO_write_ptr);
    __DO_TAINT(p->_IO_write_end);
    __DO_TAINT(p->_IO_buf_base);
    __DO_TAINT(p->_IO_buf_end);

    __DO_TAINT(p->_IO_save_base);
    __DO_TAINT(p->_IO_backup_base);
    __DO_TAINT(p->_IO_save_end);
    __DO_TAINT(p->_shortbuf);

    p->_markers->_sbuf = p;
    p->_markers->_next = p->_markers;

    // p->_codecvt;

    __DO_TAINTW(p->_wide_data->_IO_read_ptr);
    __DO_TAINTW(p->_wide_data->_IO_read_end);
    __DO_TAINTW(p->_wide_data->_IO_read_base);
    __DO_TAINTW(p->_wide_data->_IO_write_base);
    __DO_TAINTW(p->_wide_data->_IO_write_ptr);
    __DO_TAINTW(p->_wide_data->_IO_write_end);
    __DO_TAINTW(p->_wide_data->_IO_buf_base);
    __DO_TAINTW(p->_wide_data->_IO_buf_end);

    __DO_TAINTW(p->_wide_data->_IO_save_base);
    __DO_TAINTW(p->_wide_data->_IO_backup_base);
    __DO_TAINTW(p->_wide_data->_IO_save_end);

    __DO_TAINTW(p->_wide_data->_shortbuf);
}

struct _IO_FILE* __get_tainted_FILE(void)
{
    _IO_FILE* p;
    __taint_FILE(p);
    return p;
}

int remove (const char /*$untainted*/ *filename) {}
int rename (const char /*$untainted*/ *__old, const char /*$untainted*/ *__new) {}

FILE *tmpfile (void) { return __get_tainted_FILE(); }

FILE *tmpfile64 (void) { return __get_tainted_FILE(); }

char $_1 *tmpnam (char $_1 *__s) {}

char $_1 *tmpnam_r (char $_1 *__s) {}

char $_1_2 *tempnam (const char $_1 *dir, const char $_2 *__pfx) {}

int fclose (FILE *stream) {}

int fflush (FILE *stream) {}

int fflush_unlocked (FILE *stream) {}

int fcloseall (void) {}

FILE *fopen (const char /*$untainted*/ $_1 * filename, const char /*$untainted*/ $_2 * modes)
{ return __get_tainted_FILE(); }

FILE *freopen (const char /*$untainted*/ $_1 * filename, const char /*$untainted*/ $_2 * modes, FILE * stream)
{ return __get_tainted_FILE(); }

FILE *fopen64 (const char /*$untainted*/ $_1 * filename, const char /*$untainted*/ $_2 * modes)
{ return __get_tainted_FILE(); }
FILE *freopen64 (const char /*$untainted*/ $_1 * filename, const char /*$untainted*/ $_2 * modes, FILE * stream)
{ return __get_tainted_FILE(); }

FILE *fdopen (int fd, const char /*$untainted*/ $_1 *modes)
{ return __get_tainted_FILE(); }

// this is a GCC extension.  It opens a stdio stream given custom
// read/write/seek/close function pointers.  Since the user interface is still
// fread et al we don't need to do anything abnormal.
FILE *fopencookie (void * magic_cookie, const char $_1 * modes, _IO_cookie_io_functions_t io_funcs)
{ return __get_tainted_FILE(); }

// this creates a FILE stream from a buffer -- in the future we could make the
// taintedness of a FILE* depend on the taintedness of the input; probably few
// uses of this though.
FILE *fmemopen (void $tainted *__s, size_t len, const char /*$untainted*/ $_2 *modes)
{ return __get_tainted_FILE(); }

FILE *open_memstream (char $tainted ** bufloc, size_t * sizeloc)
{ return __get_tainted_FILE(); }

// buf will be written to so mark it $tainted just in case someone reads
// directly from this buffer later!
void setbuf (FILE * stream, char $tainted * buf) {}
int setvbuf (FILE * stream, char $tainted * buf, int modes, size_t n) {}

void setbuffer (FILE * stream, char $tainted * buf, size_t size) {}

void setlinebuf (FILE *stream) {}

FAKEBUILTIN
int fprintf (FILE * stream, const char $untainted * format, ...) {}

// only for __builtin_fprintf_unlocked
FAKEBUILTIN
int fprintf_unlocked (FILE * stream, const char $untainted * format, ...) {}
int vfprintf_unlocked (const char $untainted * format, va_list arg) {}

FAKEBUILTIN
int printf (const char $untainted * format, ...) {}

// only for __builtin_printf_unlocked
FAKEBUILTIN
int printf_unlocked (const char $untainted * format, ...) {}
int vprintf_unlocked (const char $untainted * format, va_list arg) {}

FAKEBUILTIN
int sprintf (char * s, const char $untainted * format, ...) {
    va_list arg;
    va_start(arg, format);
    __taint_string_from_vararg(s, arg);
    va_end(arg);
}

FAKEBUILTIN
int vfprintf (FILE * s, const char $untainted * format, va_list arg) {}

FAKEBUILTIN
int vprintf (const char $untainted * format, va_list arg) {}

FAKEBUILTIN
int vsprintf (char * s, const char $untainted * format, va_list arg) { __taint_string_from_vararg(s, arg); }

FAKEBUILTIN
int snprintf (char * s, size_t maxlen, const char $untainted * format, ...) {
    va_list arg;
    va_start(arg, format);
    __taint_string_from_vararg(s, arg);
    va_end(arg);
}

FAKEBUILTIN
int vsnprintf (char * s, size_t maxlen, const char $untainted * format, va_list arg) { __taint_string_from_vararg(s, arg); }
int vasprintf (char ** ptr, const char $untainted * f, va_list arg) { __taint_string_from_vararg(*ptr, arg); }
int __asprintf (char ** ptr, const char $untainted * fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    __taint_string_from_vararg(*ptr, arg);
    va_end(arg);
}
int asprintf (char ** ptr, const char $untainted * fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    __taint_string_from_vararg(*ptr, arg);
    va_end(arg);
}

int vdprintf (int fd, const char $untainted * fmt, va_list arg) {}
int dprintf (int fd, const char $untainted * fmt, ...) {}

FAKEBUILTIN
int fscanf (FILE * stream, const char $untainted * format, ...) {
    va_list arg;
    va_start(arg, format);
    __taint_vararg_scanf(arg);
    va_end(arg);
}

FAKEBUILTIN
int scanf (const char $untainted * format, ...) {
    va_list arg;
    va_start(arg, format);
    __taint_vararg_scanf(arg);
    va_end(arg);
}

FAKEBUILTIN
int sscanf (const char * s, const char $untainted * format, ...) {
    va_list arg;
    va_start(arg, format);
    __taint_vararg_from_string(s, arg);
    va_end(arg);
}

FAKEBUILTIN
int vfscanf (FILE * s, const char $untainted * format, va_list arg) { __taint_vararg_scanf(arg); }

FAKEBUILTIN
int vscanf (const char $untainted * format, va_list arg)  { __taint_vararg_scanf(arg); }

FAKEBUILTIN
int vsscanf (const char * s, const char $untainted * format, va_list arg) { __taint_vararg_from_string(s, arg); }

// the resulting int is used as a char.
int $tainted fgetc (FILE *stream) {}
#undef getc
int $tainted getc (FILE *stream) {}
int $tainted getchar (void) {}

// #define getc(_fp) _IO_getc (_fp)

int $tainted getc_unlocked (FILE *stream) {}
int $tainted getchar_unlocked (void) {}

int $tainted fgetc_unlocked (FILE *stream) {}

// the *putc* functions return the input character if not EOF
FAKEBUILTIN
int $_1_2 fputc (int $_1 c, FILE *stream) {}
#undef putc
int $_1_2 putc (int $_1 c, FILE *stream) {}
FAKEBUILTIN
int $_1_2 putchar (int $_1 c) {}

// #define putc(_ch, _fp) _IO_putc (_ch, _fp)

int $_1_2 fputc_unlocked (int $_1 c, FILE *stream) {}

int $_1_2 putc_unlocked (int $_1 c, FILE *stream) {}
FAKEBUILTIN
int $_1_2 putchar_unlocked (int $_1 c) {}

$tainted int getw (FILE *stream) {}

int $_1_2 putw (int $_1 w, FILE *stream) {}

char $tainted *fgets (char $tainted * s, int n, FILE * stream) {}

char $tainted *gets (char $tainted *__s) {}

char $tainted *fgets_unlocked (char $tainted * s, int n, FILE * stream) {}

_IO_ssize_t __getdelim (char $tainted ** lineptr, size_t * n, int delimiter, FILE * stream) {}
_IO_ssize_t getdelim (char $tainted ** lineptr, size_t * n, int delimiter, FILE * stream) {}

_IO_ssize_t getline (char $tainted ** lineptr, size_t * n, FILE * stream) {}

FAKEBUILTIN
int fputs (const char $_1 * s, FILE * stream) {}

FAKEBUILTIN
int puts (const char $_1 *__s) {}

// only for __builtin_puts_unlocked
FAKEBUILTIN
int puts_unlocked (const char $_1 *__s) {}

int $_1 ungetc (int $_1 c, FILE *stream) {}

size_t fread (void $tainted * ptr, size_t size, size_t n, FILE * stream) {}

FAKEBUILTIN
size_t fwrite (const void $_1 * ptr, size_t size, size_t n, FILE * s) {}

FAKEBUILTIN
int fputs_unlocked (const char $_1 * s, FILE * stream) {}

size_t fread_unlocked (void $tainted * ptr, size_t size, size_t n, FILE * stream) {}
FAKEBUILTIN
size_t fwrite_unlocked (const void $_1 * ptr, size_t size, size_t n, FILE * stream) {}

int fseek (FILE *stream, long int off, int whence) {}
long int $tainted ftell (FILE *stream) {}
void rewind (FILE *stream) {}

int fseeko (FILE *stream, __off_t off, int whence) {}

__off_t $tainted ftello (FILE *stream) {}

int fgetpos (FILE * stream, fpos_t $tainted * pos) {}

int fsetpos (FILE *stream, const fpos_t *__pos) {}

int fseeko64 (FILE *stream, __off64_t off, int whence) {}
__off64_t $tainted ftello64 (FILE *stream) {}
int fgetpos64 (FILE * stream, fpos64_t $tainted * pos) {}
int fsetpos64 (FILE *stream, const fpos64_t *__pos) {}

void clearerr (FILE *stream) {}
int feof (FILE *stream) {}
int ferror (FILE *stream) {}

void clearerr_unlocked (FILE *stream) {}
int feof_unlocked (FILE *stream) {}
int ferror_unlocked (FILE *stream) {}

void perror (const char *__s) {}

int fileno (FILE *stream) {}

int fileno_unlocked (FILE *stream) {}


FILE *popen (const char /*$untainted*/ $_1 *__command, const char /*$untainted*/ $_1 *modes)
{ return __get_tainted_FILE(); }
int pclose (FILE *stream) {}

char $tainted *ctermid (char $tainted *__s) {}

char $tainted *cuserid (char $tainted *__s) {}

struct obstack;

int obstack_printf (struct obstack * obstack, const char $untainted * format, ...) {}
int obstack_vprintf (struct obstack * obstack, const char $untainted * format, va_list args) {}

void flockfile (FILE *stream) {}

int ftrylockfile (FILE *stream) {}

void funlockfile (FILE *stream) {}


//////////////////////////////////////////////////////////////////////
// for libio.h

// struct _IO_cookie_file
// {
//     struct _IO_FILE_plus __fp;
//     void *__cookie;
//     _IO_cookie_io_functions_t __io_functions;
// };

struct _IO_cookie_file
{
    struct _IO_FILE __fp;
};

// (this shouldn't be called by the user anyway)
void _IO_cookie_init (struct _IO_cookie_file *cfile, int read_write, void *cookie, _IO_cookie_io_functions_t fns)
{
    __taint_FILE(&(cfile->__fp));
}

// NOTE: many of the stdio macros end up hinging on these:
int $tainted __underflow (_IO_FILE *fp) {}
int $tainted __uflow (_IO_FILE *fp) {}
int $tainted __overflow (_IO_FILE *fp, int c) {}
_IO_wint_t $tainted __wunderflow (_IO_FILE *fp) {}
_IO_wint_t $tainted __wuflow (_IO_FILE *fp) {}
_IO_wint_t $tainted __woverflow (_IO_FILE *fp, _IO_wint_t c) {}

// note: getc etc get inlined to these, so it's imperative the _IO_read_ptr etc
// get tainted properly!  `stdin' etc. need to be tainted!

// #define _IO_getc_unlocked(_fp)
//     ((_fp)->_IO_read_ptr >= (_fp)->_IO_read_end ? __uflow (_fp)
//      : *(unsigned char *) (_fp)->_IO_read_ptr++)
// ... etc

int $tainted _IO_getc (_IO_FILE *fp) {}
int $_1_2 _IO_putc (int $_1 c, _IO_FILE *fp) {}
int _IO_feof (_IO_FILE *fp) {}
int _IO_ferror (_IO_FILE *fp) {}

int $tainted _IO_peekc_locked (_IO_FILE *fp) {}

#undef _IO_flockfile
void _IO_flockfile (_IO_FILE *fp) {}
#undef _IO_funlockfile
void _IO_funlockfile (_IO_FILE *fp) {}
#undef _IO_ftrylockfile
int _IO_ftrylockfile (_IO_FILE *fp) {}

int _IO_vfscanf (_IO_FILE * fp, const char $untainted * format, va_list arg, int *p)
{
    __taint_vararg_scanf(arg);
}

int _IO_vfprintf (_IO_FILE *fp, const char $untainted * format, va_list arg) {}

_IO_ssize_t _IO_padn (_IO_FILE *fp, int i, _IO_ssize_t n1) {}
_IO_size_t _IO_sgetn (_IO_FILE *fp, void *p, _IO_size_t n1) {
    __DO_TAINT(p);
}

_IO_off64_t _IO_seekoff (_IO_FILE *fp, _IO_off64_t i1, int i2, int i3) {}
_IO_off64_t _IO_seekpos (_IO_FILE *fp, _IO_off64_t i1, int i2) {}

void _IO_free_backup_area (_IO_FILE *fp) {}

_IO_wint_t $tainted _IO_getwc (_IO_FILE *fp) {}
_IO_wint_t $_1_2 _IO_putwc (wchar_t $_1 wc, _IO_FILE *fp) {}
#undef _IO_fwide
int _IO_fwide (_IO_FILE *fp, int mode) {}

const int _IO_stdin_used;

int _IO_vfwscanf (_IO_FILE * fp, const wchar_t $untainted * format, va_list arg, int *p1)
{
    __taint_vararg_scanf(arg);
}

int _IO_vfwprintf (_IO_FILE *fp, const wchar_t $untainted * format, va_list arg) {}

_IO_ssize_t _IO_wpadn (_IO_FILE *fp, wint_t i1, _IO_ssize_t n1) {}
void _IO_free_wbackup_area (_IO_FILE *fp) {}


//////////////////////////////////////////////////////////////////////

// globals

struct _IO_FILE _IO_2_1_stdin_;
struct _IO_FILE _IO_2_1_stdout_;
struct _IO_FILE _IO_2_1_stderr_;

#undef stdin
#undef stdout
#undef stderr
struct _IO_FILE *stdin = &_IO_2_1_stdin_;
struct _IO_FILE *stdout = &_IO_2_1_stdout_;
struct _IO_FILE *stderr = &_IO_2_1_stderr_;

struct _IO_FILE *_IO_stdin = &_IO_2_1_stdin_;
struct _IO_FILE *_IO_stdout = &_IO_2_1_stdout_;
struct _IO_FILE *_IO_stderr = &_IO_2_1_stderr_;

void __taint_stdio_globals() {
    __taint_FILE(&_IO_2_1_stdin_);
    __taint_FILE(&_IO_2_1_stdout_);
    __taint_FILE(&_IO_2_1_stderr_);
}
