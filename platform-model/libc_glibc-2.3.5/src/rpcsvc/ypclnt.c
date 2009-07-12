// $Id$

#include <glibc-fs/prelude.h>
#include <rpcsvc/ypclnt.h>

// struct ypall_callback
//   {
//     int (*foreach) (int __status, char *__key, int __keylen,
// 		    char *__val, int __vallen, char *__data);
//     char *data;
//   };

int yp_bind (const char *x) {}
void yp_unbind (const char * x) {}

int yp_get_default_domain (char $tainted **s1) {}

int yp_match (const char *s1, const char *s2, const char *s3,
              const int i1, char $tainted **s4, int *i2) {}

int yp_first (const char *s1, const char *s2, char $tainted **s3,
              int *i1, char $tainted **s4, int *i5) {}

int yp_next (const char *s1, const char *s2, const char *s3,
             const int i1, char $tainted **s4, int *i2,
             char $tainted **s5, int *i3) {}

int yp_master (const char *s1, const char *s2, char $tainted **s3) {}

int yp_order (const char *s1, const char *s2, unsigned int *i1) {}

int yp_all (const char *s1, const char *s2, const struct ypall_callback *cb) {
    char $tainted *t1;
    char $tainted *t2;
    cb->foreach(0, t1, 0, t2, 0, cb->data);
}

const char *yperr_string (const int i1) {}

const char *ypbinderr_string (const int i2) {}

int ypprot_err (const int i3) {}

int yp_update (char *s1, char *s2, unsigned int i1, char *s3,
               int i2, char *s4, int i5) {}

// quarl 2006-07-11
//    These are undocumented :(

// int yp_maplist (const char *, struct ypmaplist **);

// int __yp_check (char **);
