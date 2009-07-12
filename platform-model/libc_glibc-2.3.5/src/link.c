#include <glibc-fs/prelude.h>
#include <link.h>

// struct r_debug
// {
//     int r_version;

//     struct link_map *r_map;

//     ElfW(Addr) r_brk;
//     enum
//     {

// 	RT_CONSISTENT,
// 	RT_ADD,
// 	RT_DELETE
//     } r_state;

//     ElfW(Addr) r_ldbase;
// };

// struct link_map
// {

//     ElfW(Addr) l_addr;
//     char *l_name;
//     ElfW(Dyn) *l_ld;
//     struct link_map *l_next, *l_prev;
// };

// struct dl_phdr_info
// {
//     ElfW(Addr) dlpi_addr;
//     const char *dlpi_name;
//     const ElfW(Phdr) *dlpi_phdr;
//     ElfW(Half) dlpi_phnum;
// };

static inline void __taint_dl_phdr_info(struct dl_phdr_info* p)
{/*T:H*/
    __DO_TAINTX(&(p->dlpi_addr), ElfW(Addr));
    __DO_TAINT(p->dlpi_name);
    __DO_TAINTX(p->dlpi_phdr, ElfW(Phdr));
    __DO_TAINTX(&(p->dlpi_phnum), ElfW(Half));
}

static inline struct dl_phdr_info * __get_tainted_dl_phdr_info(void)
{/*T:H*/
    struct dl_phdr_info * p;
    __taint_dl_phdr_info(p);
    return p;
}

int dl_iterate_phdr (int (*callback) (struct dl_phdr_info *info, size_t size, void *data), void *data)
{
    struct dl_phdr_info *p = __get_tainted_dl_phdr_info();
    (*callback)(p, 0, data);
}
