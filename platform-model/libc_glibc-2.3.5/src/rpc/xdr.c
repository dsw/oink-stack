// $Id$

#include <glibc-fs/prelude.h>
#include <rpc/xdr.h>

// typedef struct XDR XDR;
// struct XDR
// {
//     enum xdr_op x_op;
//     struct xdr_ops
//     {
// 	bool_t (*x_getlong) (XDR *xdrs, long *lp);
// 	bool_t (*x_putlong) (XDR *xdrs, const long *lp);
// 	bool_t (*x_getbytes) (XDR *xdrs, caddr_t addr, u_int len);
// 	bool_t (*x_putbytes) (XDR *xdrs, const char *addr, u_int len);
// 	u_int (*x_getpostn) (const XDR *xdrs);
// 	bool_t (*x_setpostn) (XDR *xdrs, u_int pos);
// 	int32_t *(*x_inline) (XDR *xdrs, u_int len);
// 	void (*x_destroy) (XDR *xdrs);
// 	bool_t (*x_getint32) (XDR *xdrs, int32_t *ip);
// 	bool_t (*x_putint32) (XDR *xdrs, const int32_t *ip);
//     }
//     *x_ops;
//     caddr_t x_public;
//     caddr_t x_private;
//     caddr_t x_base;
//     u_int x_handy;
// };

// typedef bool_t (*xdrproc_t) (XDR *, void *,...);

// struct xdr_discrim
// {
//     int value;
//     xdrproc_t proc;
// };

bool_t xdr_void (void) {}
bool_t xdr_short (XDR *xdrs, short *sp) {}
bool_t xdr_u_short (XDR *xdrs, u_short *usp) {}
bool_t xdr_int (XDR *xdrs, int *ip) {}
bool_t xdr_u_int (XDR *xdrs, u_int *up) {}
bool_t xdr_long (XDR *xdrs, long *lp) {}
bool_t xdr_u_long (XDR *xdrs, u_long *ulp) {}
bool_t xdr_hyper (XDR *xdrs, quad_t *llp) {}
bool_t xdr_u_hyper (XDR *xdrs, u_quad_t *ullp) {}
bool_t xdr_longlong_t (XDR *xdrs, quad_t *llp) {}
bool_t xdr_u_longlong_t (XDR *xdrs, u_quad_t *ullp) {}
bool_t xdr_int8_t (XDR *xdrs, int8_t *ip) {}
bool_t xdr_uint8_t (XDR *xdrs, uint8_t *up) {}
bool_t xdr_int16_t (XDR *xdrs, int16_t *ip) {}
bool_t xdr_uint16_t (XDR *xdrs, uint16_t *up) {}
bool_t xdr_int32_t (XDR *xdrs, int32_t *ip) {}
bool_t xdr_uint32_t (XDR *xdrs, uint32_t *up) {}
bool_t xdr_int64_t (XDR *xdrs, int64_t *ip) {}
bool_t xdr_uint64_t (XDR *xdrs, uint64_t *up) {}
bool_t xdr_bool (XDR *xdrs, bool_t *bp) {}
bool_t xdr_enum (XDR *xdrs, enum_t *ep) {}
bool_t xdr_array (XDR * _xdrs, caddr_t *addrp, u_int *__sizep, u_int maxsize, u_int elsize, xdrproc_t elproc) {}
bool_t xdr_bytes (XDR *xdrs, char  **cpp, u_int *__sizep, u_int maxsize) {}
bool_t xdr_opaque (XDR *xdrs, caddr_t cp, u_int cnt) {}
bool_t xdr_string (XDR *xdrs, char $tainted **cpp, u_int maxsize) {}
bool_t xdr_union (XDR *__xdrs, enum_t *dscmp, char *unp, const struct xdr_discrim *choices, xdrproc_t dfault) {}
bool_t xdr_char (XDR *xdrs, char $tainted *cp) {}
bool_t xdr_u_char (XDR *xdrs, u_char $tainted *cp) {}
bool_t xdr_vector (XDR *xdrs, char $tainted *basep, u_int nelem, u_int elemsize, xdrproc_t xdr_elem) {}
bool_t xdr_float (XDR *xdrs, float *fp) {}
bool_t xdr_double (XDR *xdrs, double *dp) {}
bool_t xdr_reference (XDR *xdrs, caddr_t *xpp, u_int size, xdrproc_t proc) {}
bool_t xdr_pointer (XDR *xdrs, char $tainted **objpp, u_int obj_size, xdrproc_t xdr_obj) {}
bool_t xdr_wrapstring (XDR *xdrs, char $tainted **cpp) {}
u_long xdr_sizeof (xdrproc_t s, void *p) {}

// struct netobj
// {
//     u_int n_len;
//     char *n_bytes;
// };
// typedef struct netobj netobj;

bool_t xdr_netobj (XDR *xdrs, struct netobj *np)
{
    char $tainted t;
    np->n_bytes[0] = t;
}

void xdrmem_create (XDR *xdrs, const caddr_t addr, u_int size, enum xdr_op xop)
{
    // unsound if actually used as a string - fread bug
    char $tainted t;
    *((char*)addr) = t;
}

void __taint_FILE(struct _IO_FILE* p);              // stdio.c

void xdrstdio_create (XDR *xdrs, FILE *file, enum xdr_op xop) { __taint_FILE(file); }

void xdrrec_create (XDR *xdrs, u_int sendsize, u_int recvsize, caddr_t tcp_handle,
                    int (*readit) (char *, char *, int),
                    int (*writeit) (char *, char *, int))
{
    char $tainted *t1;
    char $tainted *t2;
    char $tainted *t3;
    char $tainted *t4;
    char $tainted *t5;
    readit(t1, t2, 0);
    writeit(t3, t4, 0);
    *((char*)tcp_handle) = *t5;                      // fread bug
}

bool_t xdrrec_endofrecord (XDR *xdrs, bool_t sendnow) {}

bool_t xdrrec_skiprecord (XDR *xdrs) {}

bool_t xdrrec_eof (XDR *xdrs) {}

void xdr_free (xdrproc_t proc, char *objp) {}
