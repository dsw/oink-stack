#ifndef GLIBCFS_IGNORE_INCOMPLETE

typedef char fhandle[FHSIZE];

struct fhstatus {
    u_int fhs_status;
    union {
        fhandle fhs_fhandle;
    } fhstatus_u;
};
typedef struct fhstatus fhstatus;

typedef char *dirpath;

typedef char *name;

typedef struct mountbody *mountlist;

struct mountbody {
    name ml_hostname;
    dirpath ml_directory;
    mountlist ml_next;
};
typedef struct mountbody mountbody;

typedef struct groupnode *groups;

struct groupnode {
    name gr_name;
    groups gr_next;
};
typedef struct groupnode groupnode;

typedef struct exportnode *exports;

struct exportnode {
    dirpath ex_dir;
    groups ex_groups;
    exports ex_next;
};
typedef struct exportnode exportnode;

void * mountproc_null_1(void *, CLIENT *);
void * mountproc_null_1_svc(void *, struct svc_req *);

fhstatus * mountproc_mnt_1(dirpath *, CLIENT *);
fhstatus * mountproc_mnt_1_svc(dirpath *, struct svc_req *);

mountlist * mountproc_dump_1(void *, CLIENT *);
mountlist * mountproc_dump_1_svc(void *, struct svc_req *);

void * mountproc_umnt_1(dirpath *, CLIENT *);
void * mountproc_umnt_1_svc(dirpath *, struct svc_req *);

void * mountproc_umntall_1(void *, CLIENT *);
void * mountproc_umntall_1_svc(void *, struct svc_req *);

exports * mountproc_export_1(void *, CLIENT *);
exports * mountproc_export_1_svc(void *, struct svc_req *);

exports * mountproc_exportall_1(void *, CLIENT *);
exports * mountproc_exportall_1_svc(void *, struct svc_req *);
int mountprog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

bool_t xdr_fhandle (XDR *, fhandle);
bool_t xdr_fhstatus (XDR *, fhstatus*);
bool_t xdr_dirpath (XDR *, dirpath*);
bool_t xdr_name (XDR *, name*);
bool_t xdr_mountlist (XDR *, mountlist*);
bool_t xdr_mountbody (XDR *, mountbody*);
bool_t xdr_groups (XDR *, groups*);
bool_t xdr_groupnode (XDR *, groupnode*);
bool_t xdr_exports (XDR *, exports*);
bool_t xdr_exportnode (XDR *, exportnode*);

#endif
