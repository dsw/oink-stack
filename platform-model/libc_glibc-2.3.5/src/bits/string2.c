// $Id$

// see ../string.c for definitions for functions.

// Argument for soundness despite inline assembly:
//    The use of inline assembly in this file is of the form:
//        foo ? strcpy_asm(dest,src) : strcpy(dest,src)
//    Since we are flow-insensitive, as long as strcpy is correct, the above
//    expression is correct even if strcpy_asm has a no-op body.

// Most of the definitions here are for 'extern inline' definitions, so with
// the right flags, they wouldn't matter anyway.  I believe __strsep_g is the
// only one we do need.

#include <glibc-fs/prelude.h>
#include <bits/types.h>

void *
__mempcpy_small (void *dest1,
		 char src0_1, char src2_1, char src4_1, char src6_1,
		 __uint16_t src0_2, __uint16_t src4_2,
		 __uint32_t src0_4, __uint32_t src4_4,
		 size_t srclen)
{
    long *dest = dest1;
    *dest = src0_1;
    *dest = src2_1;
    *dest = src4_1;
    *dest = src6_1;
    *dest = src0_2;
    *dest = src4_2;
    *dest = src0_4;
    *dest = src4_4;
    return dest;
}

// in string.c
// void *__rawmemchr (const void *s, int c) { return (void*) s; }

char *
__strcpy_small (char *dest,
		__uint16_t src0_2, __uint16_t src4_2,
		__uint32_t src0_4, __uint32_t src4_4,
		size_t srclen)
{
    *dest = src0_2;
    *dest = src4_2;
    *dest = src0_4;
    *dest = src4_4;
    return dest;
}

char *
__stpcpy_small (char *dest,
		__uint16_t src0_2, __uint16_t src4_2,
		__uint32_t src0_4, __uint32_t src4_4,
		size_t srclen)
{
    *dest = src0_2;
    *dest = src4_2;
    *dest = src0_4;
    *dest = src4_4;
    return dest;
}

size_t
__strcspn_c1 (const char *__s, int __reject)
{
    register size_t __result = 0;
    while (__s[__result] != '\0' && __s[__result] != __reject)
        ++__result;
    return __result;
}

size_t
__strcspn_c2 (const char *__s, int __reject1, int __reject2)
{
    register size_t __result = 0;
    while (__s[__result] != '\0' && __s[__result] != __reject1
           && __s[__result] != __reject2)
        ++__result;
    return __result;
}

size_t
__strcspn_c3 (const char *__s, int __reject1, int __reject2,
	      int __reject3)
{
    register size_t __result = 0;
    while (__s[__result] != '\0' && __s[__result] != __reject1
           && __s[__result] != __reject2 && __s[__result] != __reject3)
        ++__result;
    return __result;
}


size_t
__strspn_c1 (const char *__s, int __accept)
{
    register size_t __result = 0;
    /* Please note that __accept never can be '\0'.  */
    while (__s[__result] == __accept)
        ++__result;
    return __result;
}

size_t
__strspn_c2 (const char *__s, int __accept1, int __accept2)
{
    register size_t __result = 0;
    /* Please note that __accept1 and __accept2 never can be '\0'.  */
    while (__s[__result] == __accept1 || __s[__result] == __accept2)
        ++__result;
    return __result;
}

size_t
__strspn_c3 (const char *__s, int __accept1, int __accept2, int __accept3)
{
    register size_t __result = 0;
    /* Please note that __accept1 to __accept3 never can be '\0'.  */
    while (__s[__result] == __accept1 || __s[__result] == __accept2
           || __s[__result] == __accept3)
        ++__result;
    return __result;
}

char *
__strpbrk_c2 (const char *__s, int __accept1, int __accept2)
{
    /* Please note that __accept1 and __accept2 never can be '\0'.  */
    while (*__s != '\0' && *__s != __accept1 && *__s != __accept2)
        ++__s;
    return *__s == '\0' ? NULL : (char *) (size_t) __s;
}

char *
__strpbrk_c3 (const char *__s, int __accept1, int __accept2,
	      int __accept3)
{
  /* Please note that __accept1 to __accept3 never can be '\0'.  */
  while (*__s != '\0' && *__s != __accept1 && *__s != __accept2
	 && *__s != __accept3)
    ++__s;
  return *__s == '\0' ? NULL : (char *) (size_t) __s;
}


char *
__strtok_r_1c (char *__s, char __sep, char **__nextp)
{
    char *__result;
    if (__s == NULL)
        __s = *__nextp;
    while (*__s == __sep)
        ++__s;
    __result = NULL;
    if (*__s != '\0')
    {
        __result = __s++;
        while (*__s != '\0')
            if (*__s++ == __sep)
            {
                __s[-1] = '\0';
                break;
            }
        *__nextp = __s;
    }
    return __result;
}


char $_1_2 * __strsep_g (char $_1_2 * $untainted * stringp, const char $_2 * delim)
{
}

extern char *strchr (const char *, int c);

char *
__strsep_1c (char **__s, char __reject)
{
    register char *__retval = *__s;
    if (__retval != NULL && (*__s = strchr (__retval, __reject)) != NULL)
        *(*__s)++ = '\0';
    return __retval;
}

char *
__strsep_2c (char **__s, char __reject1, char __reject2)
{
    register char *__retval = *__s;
    if (__retval != NULL)
    {
        register char *__cp = __retval;
        while (1)
	{
            if (*__cp == '\0')
	    {
                __cp = NULL;
                break;
	    }
            if (*__cp == __reject1 || *__cp == __reject2)
	    {
                *__cp++ = '\0';
                break;
	    }
            ++__cp;
	}
        *__s = __cp;
    }
    return __retval;
}

char *
__strsep_3c (char **__s, char __reject1, char __reject2, char __reject3)
{
    register char *__retval = *__s;
    if (__retval != NULL)
    {
        register char *__cp = __retval;
        while (1)
	{
            if (*__cp == '\0')
	    {
                __cp = NULL;
                break;
	    }
            if (*__cp == __reject1 || *__cp == __reject2 || *__cp == __reject3)
	    {
                *__cp++ = '\0';
                break;
	    }
            ++__cp;
	}
        *__s = __cp;
    }
    return __retval;
}
