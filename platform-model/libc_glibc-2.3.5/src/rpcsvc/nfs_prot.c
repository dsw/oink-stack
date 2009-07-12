#ifndef GLIBCFS_IGNORE_INCOMPLETE

#include <rpc/nfs_prot.h>

// struct nfs_fh {
//     char data[NFS_FHSIZE];
// };
// typedef struct nfs_fh nfs_fh;

// struct nfstime {
//     u_int seconds;
//     u_int useconds;
// };
// typedef struct nfstime nfstime;

// struct fattr {
//     ftype type;
//     u_int mode;
//     u_int nlink;
//     u_int uid;
//     u_int gid;
//     u_int size;
//     u_int blocksize;
//     u_int rdev;
//     u_int blocks;
//     u_int fsid;
//     u_int fileid;
//     nfstime atime;
//     nfstime mtime;
//     nfstime ctime;
// };
// typedef struct fattr fattr;

// struct sattr {
//     u_int mode;
//     u_int uid;
//     u_int gid;
//     u_int size;
//     nfstime atime;
//     nfstime mtime;
// };
// typedef struct sattr sattr;

// typedef char *filename;

// typedef char *nfspath;

// struct attrstat {
//     nfsstat status;
//     union {
//         fattr attributes;
//     } attrstat_u;
// };
// typedef struct attrstat attrstat;

// struct sattrargs {
//     nfs_fh file;
//     sattr attributes;
// };
// typedef struct sattrargs sattrargs;

// struct diropargs {
//     nfs_fh dir;
//     filename name;
// };
// typedef struct diropargs diropargs;

// struct diropokres {
//     nfs_fh file;
//     fattr attributes;
// };
// typedef struct diropokres diropokres;

// struct diropres {
//     nfsstat status;
//     union {
//         diropokres diropres;
//     } diropres_u;
// };
// typedef struct diropres diropres;

// struct readlinkres {
//     nfsstat status;
//     union {
//         nfspath data;
//     } readlinkres_u;
// };
// typedef struct readlinkres readlinkres;

// struct readargs {
//     nfs_fh file;
//     u_int offset;
//     u_int count;
//     u_int totalcount;
// };
// typedef struct readargs readargs;

// struct readokres {
//     fattr attributes;
//     struct {
//         u_int data_len;
//         char *data_val;
//     } data;
// };
// typedef struct readokres readokres;

// struct readres {
//     nfsstat status;
//     union {
//         readokres reply;
//     } readres_u;
// };
// typedef struct readres readres;

// struct writeargs {
//     nfs_fh file;
//     u_int beginoffset;
//     u_int offset;
//     u_int totalcount;
//     struct {
//         u_int data_len;
//         char *data_val;
//     } data;
// };
// typedef struct writeargs writeargs;

// struct createargs {
//     diropargs where;
//     sattr attributes;
// };
// typedef struct createargs createargs;

// struct renameargs {
//     diropargs from;
//     diropargs to;
// };
// typedef struct renameargs renameargs;

// struct linkargs {
//     nfs_fh from;
//     diropargs to;
// };
// typedef struct linkargs linkargs;

// struct symlinkargs {
//     diropargs from;
//     nfspath to;
//     sattr attributes;
// };
// typedef struct symlinkargs symlinkargs;

// typedef char nfscookie[NFS_COOKIESIZE];

// struct readdirargs {
//     nfs_fh dir;
//     nfscookie cookie;
//     u_int count;
// };
// typedef struct readdirargs readdirargs;

// struct entry {
//     u_int fileid;
//     filename name;
//     nfscookie cookie;
//     struct entry *nextentry;
// };
// typedef struct entry entry;

// struct dirlist {
//     entry *entries;
//     bool_t eof;
// };
// typedef struct dirlist dirlist;

// struct readdirres {
//     nfsstat status;
//     union {
//         dirlist reply;
//     } readdirres_u;
// };
// typedef struct readdirres readdirres;

// struct statfsokres {
//     u_int tsize;
//     u_int bsize;
//     u_int blocks;
//     u_int bfree;
//     u_int bavail;
// };
// typedef struct statfsokres statfsokres;

// struct statfsres {
//     nfsstat status;
//     union {
//         statfsokres reply;
//     } statfsres_u;
// };
// typedef struct statfsres statfsres;


void * nfsproc_null_2(void *, CLIENT *);
void * nfsproc_null_2_svc(void *, struct svc_req *);

attrstat * nfsproc_getattr_2(nfs_fh *, CLIENT *);
attrstat * nfsproc_getattr_2_svc(nfs_fh *, struct svc_req *);

attrstat * nfsproc_setattr_2(sattrargs *, CLIENT *);
attrstat * nfsproc_setattr_2_svc(sattrargs *, struct svc_req *);

void * nfsproc_root_2(void *, CLIENT *);
void * nfsproc_root_2_svc(void *, struct svc_req *);

diropres * nfsproc_lookup_2(diropargs *, CLIENT *);
diropres * nfsproc_lookup_2_svc(diropargs *, struct svc_req *);

readlinkres * nfsproc_readlink_2(nfs_fh *, CLIENT *);
readlinkres * nfsproc_readlink_2_svc(nfs_fh *, struct svc_req *);

readres * nfsproc_read_2(readargs *, CLIENT *);
readres * nfsproc_read_2_svc(readargs *, struct svc_req *);

void * nfsproc_writecache_2(void *, CLIENT *);
void * nfsproc_writecache_2_svc(void *, struct svc_req *);

attrstat * nfsproc_write_2(writeargs *, CLIENT *);
attrstat * nfsproc_write_2_svc(writeargs *, struct svc_req *);

diropres * nfsproc_create_2(createargs *, CLIENT *);
diropres * nfsproc_create_2_svc(createargs *, struct svc_req *);

nfsstat * nfsproc_remove_2(diropargs *, CLIENT *);
nfsstat * nfsproc_remove_2_svc(diropargs *, struct svc_req *);

nfsstat * nfsproc_rename_2(renameargs *, CLIENT *);
nfsstat * nfsproc_rename_2_svc(renameargs *, struct svc_req *);

nfsstat * nfsproc_link_2(linkargs *, CLIENT *);
nfsstat * nfsproc_link_2_svc(linkargs *, struct svc_req *);

nfsstat * nfsproc_symlink_2(symlinkargs *, CLIENT *);
nfsstat * nfsproc_symlink_2_svc(symlinkargs *, struct svc_req *);

diropres * nfsproc_mkdir_2(createargs *, CLIENT *);
diropres * nfsproc_mkdir_2_svc(createargs *, struct svc_req *);

nfsstat * nfsproc_rmdir_2(diropargs *, CLIENT *);
nfsstat * nfsproc_rmdir_2_svc(diropargs *, struct svc_req *);

readdirres * nfsproc_readdir_2(readdirargs *, CLIENT *);
readdirres * nfsproc_readdir_2_svc(readdirargs *, struct svc_req *);

statfsres * nfsproc_statfs_2(nfs_fh *, CLIENT *);
statfsres * nfsproc_statfs_2_svc(nfs_fh *, struct svc_req *);
int nfs_program_2_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

bool_t xdr_nfsstat (XDR *, nfsstat*);
bool_t xdr_ftype (XDR *, ftype*);
bool_t xdr_nfs_fh (XDR *, nfs_fh*);
bool_t xdr_nfstime (XDR *, nfstime*);
bool_t xdr_fattr (XDR *, fattr*);
bool_t xdr_sattr (XDR *, sattr*);
bool_t xdr_filename (XDR *, filename*);
bool_t xdr_nfspath (XDR *, nfspath*);
bool_t xdr_attrstat (XDR *, attrstat*);
bool_t xdr_sattrargs (XDR *, sattrargs*);
bool_t xdr_diropargs (XDR *, diropargs*);
bool_t xdr_diropokres (XDR *, diropokres*);
bool_t xdr_diropres (XDR *, diropres*);
bool_t xdr_readlinkres (XDR *, readlinkres*);
bool_t xdr_readargs (XDR *, readargs*);
bool_t xdr_readokres (XDR *, readokres*);
bool_t xdr_readres (XDR *, readres*);
bool_t xdr_writeargs (XDR *, writeargs*);
bool_t xdr_createargs (XDR *, createargs*);
bool_t xdr_renameargs (XDR *, renameargs*);
bool_t xdr_linkargs (XDR *, linkargs*);
bool_t xdr_symlinkargs (XDR *, symlinkargs*);
bool_t xdr_nfscookie (XDR *, nfscookie);
bool_t xdr_readdirargs (XDR *, readdirargs*);
bool_t xdr_entry (XDR *, entry*);
bool_t xdr_dirlist (XDR *, dirlist*);
bool_t xdr_readdirres (XDR *, readdirres*);
bool_t xdr_statfsokres (XDR *, statfsokres*);
bool_t xdr_statfsres (XDR *, statfsres*);

#endif
