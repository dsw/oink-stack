/* see License.txt for copyright and terms of use */

#ifndef PROFILE_H
#define PROFILE_H

/* Should only be included in regions.h and profile.c */

void *profile_typed_ralloc(region r, size_t size, type_t type, char *file, int line);
void *profile_typed_rarrayalloc(region r, size_t n, size_t size, type_t type, char *file, int line);
void *profile_typed_rarrayextend(region r, void *old, size_t n, size_t size, type_t type, char *file, int line);
char *profile_rstralloc(region r, size_t size, char *file, int line);
char *profile_rstralloc0(region r, size_t size, char *file, int line);
char *profile_rstrdup(region r, const char *s, char *file, int line);
char *profile_rstrextend(region r, const char *old, size_t newsize, char *file, int line);
char *profile_rstrextend0(region r, const char *old, size_t newsize, char *file, int line);
void *profile__rcralloc_small0(region r, size_t size, char *file, int line);

#ifdef REGION_PROFILE
#define typed_ralloc(r, size, type) profile_typed_ralloc(r, size, type, __FILE__, __LINE__)
#define typed_rarrayalloc(r, n, size, type) profile_typed_rarrayalloc(r, n, size, type, __FILE__, __LINE__)
#define typed_rarrayextend(r, old, n, size, type) profile_typed_rarrayextend(r, old, n, size, type, __FILE__, __LINE__)

#define rstralloc(r, size) profile_rstralloc(r, size, __FILE__, __LINE__)
#define rstralloc0(r, size) profile_rstralloc0(r, size, __FILE__, __LINE__)
#define rstrdup(r, s) profile_rstrdup(r, s, __FILE__, __LINE__)

#define rstrextend(r, old, newsize) profile_rstrextend(r, old, newsize, __FILE__, __LINE__)
#define rstrextend0(r, old, newsize) profile_rstrextend0(r, old, newsize, __FILE__, __LINE__)
#define __rcralloc_small0(r, size) profile__rcralloc_small0(r, size, __FILE__, __LINE__)
#endif

void profile(void);

#endif
