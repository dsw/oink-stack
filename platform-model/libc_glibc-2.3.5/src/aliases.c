#include <glibc-fs/prelude.h>
#include <aliases.h>

// /* Structure to represent one entry of the alias data base.  */
// struct aliasent
//   {
//     char *alias_name;
//     size_t alias_members_len;
//     char **alias_members;
//     int alias_local;
//   };


void setaliasent (void) {}

void endaliasent (void) {}

static inline void __taint_aliasent(struct aliasent* p)
{/*T:H*/
    char $tainted c;
    p->alias_name[0] = c;
    p->alias_members[0][0] = c;
}

static inline struct aliasent* __get_tainted_aliasent()
{/*T:H*/
    struct aliasent* p;
    __taint_aliasent(p);
    return p;
}


struct aliasent *getaliasent (void) {
    return __get_tainted_aliasent();
}

int getaliasent_r (struct aliasent * result_buf, char $tainted * buffer, size_t buflen, struct aliasent ** result)
{
    __taint_aliasent(result_buf);
    result[0] = result_buf;
}

struct aliasent *getaliasbyname (const char $tainted *name)
{ return __get_tainted_aliasent(); }

int getaliasbyname_r (const char * name, struct aliasent * result_buf,
                      char $tainted * buffer, size_t buflen, struct aliasent ** result)
{
    __taint_aliasent(result_buf);
    result[0] = result_buf;
}

