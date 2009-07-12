// $Id$

#include <glibc-fs/prelude.h>
#include <rpc/pmap_rmt.h>

// struct rmtcallargs {
//     u_long prog, vers, proc, arglen;
//     caddr_t args_ptr;
//     xdrproc_t xdr_args;
// };

bool_t xdr_rmtcall_args (XDR *xdrs, struct rmtcallargs *crp) {}

// struct rmtcallres {
//     u_long *port_ptr;
//     u_long resultslen;
//     caddr_t results_ptr;
//     xdrproc_t xdr_results;
// };

bool_t xdr_rmtcallres (XDR *xdrs, struct rmtcallres *crp) {}
