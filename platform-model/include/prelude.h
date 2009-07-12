// $Id: prelude.h 2824 2006-03-09 03:06:31Z quarl $

#ifdef ELIDE_QUALIFIERS
# define $tainted /*nothing*/
# define $untainted /*nothing*/
# define $_1
# define $_2
# define $_1_2
# define $_3
# define $_1_2_3
#endif

// get all declarations as if we are compiling glibc itself
#define _GNU_SOURCE 1

#include <features.h>

#define _LIBC 1                                  // must come after features.h
#define attribute_hidden

#include <stdarg.h>
#include <wchar.h>

// Taint the value pointed to by variable X of type T*.  N is any symbol name;
// it will be visible in the error message.
#define __DO_TAINTXN(X,T,N) do {                                  \
        T $tainted N;                                             \
        *((T*) (X)) = N;                                          \
    } while(0)

#define __DO_TAINTX(X,T) __DO_TAINTXN(X,T,TAINTED_FROM_ENVIRONMENT)

#define __DO_TAINT(X) __DO_TAINTX(X,char)

#define __DO_TAINTW(X) __DO_TAINTX(X,wchar_t)

// TODO: complete after defining the set of all types for va_arg

// TODO: make sure flowing taint both *to* and *from* va_list/... works

// quarl 2006-06-15
//    We only flow taint from char (and wchar_t) because otherwise this leads
//    to common false positives:
//         int $tainted n;
//         sprintf(fmt, "%%%ds", n);
//         printf(fmt, foo);

// usage: define __FLOW_TAINT around this macro
#define __FLOW_TAINT_VARARG_TYPES() do                            \
    {                                                             \
        __FLOW_TAINT(char);                                       \
        __FLOW_TAINT(wchar_t);                                    \
    } while(0)

        // __FLOW_TAINT(int);
        // __FLOW_TAINT(float);
        // __FLOW_TAINT(double);


static inline void __taint_string_from_vararg(char* s, va_list arg)
{
#define __FLOW_TAINT(T) do                                        \
    {                                                             \
        T* p = va_arg(arg, T*);                                   \
        s[0] = (char) *p;                                         \
    } while(0)

    __FLOW_TAINT_VARARG_TYPES();
#undef __FLOW_TAINT
}

static inline void __taint_wstring_from_vararg(wchar_t* s, va_list arg)
{
#define __FLOW_TAINT(T) do                                        \
    {                                                             \
        T* p = va_arg(arg, T*);                                   \
        s[0] = (wchar_t) *p;                                      \
    } while(0)

    __FLOW_TAINT_VARARG_TYPES();
#undef __FLOW_TAINT
}

static inline void __taint_vararg_from_string(char const* s, va_list arg)
{
#define __FLOW_TAINT(T) do                                        \
    {                                                             \
        T* p = va_arg(arg, T*);                                   \
        *p = (T) *s;                                              \
    } while(0)

    __FLOW_TAINT_VARARG_TYPES();
#undef __FLOW_TAINT
}

static inline void __taint_vararg_from_wstring(wchar_t const* s, va_list arg)
{
#define __FLOW_TAINT(T) do                                        \
    {                                                             \
        T* p = va_arg(arg, T*);                                   \
        *p = (T) *s;                                              \
    } while(0)

    __FLOW_TAINT_VARARG_TYPES();
#undef __FLOW_TAINT
}

static inline void __taint_vararg_environment(va_list arg)
{
#define __FLOW_TAINT(T) do                                        \
    {                                                             \
        T* p = va_arg(arg, T*);                                   \
        __DO_TAINTXN(p, T, TAINTED_FROM_ENVIRONMENT);             \
    } while(0)

    __FLOW_TAINT_VARARG_TYPES();
#undef __FLOW_TAINT
}

static inline void __taint_vararg_scanf(va_list arg)
{
#define __FLOW_TAINT(T) do                                        \
    {                                                             \
        T* p = va_arg(arg, T*);                                   \
        __DO_TAINTXN(p, T, TAINTED_FROM_SCANF);                   \
    } while(0)

    __FLOW_TAINT_VARARG_TYPES();
#undef __FLOW_TAINT
}

#ifdef COMPILE_GCC
// this is just used to squelch gcc warnings
extern void __void_noreturn (void) __attribute__ ((__noreturn__));
#else
static inline void __void_noreturn(void) {}
#endif

// GCC fake builtins -- see doc/builtins.txt
#define FAKEBUILTIN /*nothing*/
