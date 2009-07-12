// $Id$

#include <glibc-fs/prelude.h>
#include <rpc/pmap_prot.h>

// struct pmap {
//     long unsigned pm_prog;
//     long unsigned pm_vers;
//     long unsigned pm_prot;
//     long unsigned pm_port;
// };

bool_t xdr_pmap (XDR *xdrs, struct pmap *regs) {}

// struct pmaplist {
//     struct pmap	pml_map;
//     struct pmaplist *pml_next;
// };

bool_t xdr_pmaplist (XDR *xdrs, struct pmaplist **rp) {}
