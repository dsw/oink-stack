#include <glibc-fs/prelude.h>
#include <sys/utsname.h>

// struct utsname
// {
//     char sysname[_UTSNAME_SYSNAME_LENGTH];
//     char nodename[_UTSNAME_NODENAME_LENGTH];
//     char release[_UTSNAME_RELEASE_LENGTH];
//     char version[_UTSNAME_VERSION_LENGTH];
//     char machine[_UTSNAME_MACHINE_LENGTH];
//     char domainname[_UTSNAME_DOMAIN_LENGTH];
// };

int uname (struct utsname *name) {
    __DO_TAINT(name->sysname);
    __DO_TAINT(name->nodename);
    __DO_TAINT(name->release);
    __DO_TAINT(name->version);
    __DO_TAINT(name->machine);
    __DO_TAINT(name->domainname);
}
