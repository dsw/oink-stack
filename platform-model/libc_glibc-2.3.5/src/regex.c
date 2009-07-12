#include <glibc-fs/prelude.h>
#include <regex.h>

// reg_syntax_t re_syntax_options;

// struct re_pattern_buffer
// {
//     unsigned char *buffer;
//     unsigned long int allocated;
//     unsigned long int used;
//     reg_syntax_t syntax;
//     char *fastmap;
//     RE_TRANSLATE_TYPE translate;
//     size_t re_nsub;
//     unsigned can_be_null : 1;
//     unsigned regs_allocated : 2;
//     unsigned fastmap_accurate : 1;
//     unsigned no_sub : 1;
//     unsigned not_bol : 1;
//     unsigned not_eol : 1;
//     unsigned newline_anchor : 1;
// };

// typedef struct re_pattern_buffer regex_t;
// typedef int regoff_t;
// struct re_registers
// {
//     unsigned num_regs;
//     regoff_t *start;
//     regoff_t *end;
// };
// typedef struct
// {
//     regoff_t rm_so;
//     regoff_t rm_eo;
// } regmatch_t;

static inline void __taint_re_pattern_buffer(struct re_pattern_buffer* p)
{/*T:H*/
    __DO_TAINT(p->buffer);
    __DO_TAINT(p->fastmap);
    __DO_TAINT(p->translate);
}

reg_syntax_t re_set_syntax (reg_syntax_t syntax) {}

const char $_1 * re_compile_pattern (const char $_1 * pattern, size_t length,
                                     struct re_pattern_buffer *buffer)
{ __taint_re_pattern_buffer(buffer); }

int re_compile_fastmap (struct re_pattern_buffer *buffer)
{ __taint_re_pattern_buffer(buffer); }

int re_search (struct re_pattern_buffer *buffer, const char $_2 *string,
               int length, int start, int range, struct re_registers *regs) {}

int re_search_2 (struct re_pattern_buffer *buffer, const char *string1,
                 int length1, const char *string2, int length2,
                 int start, int range, struct re_registers *regs, int stop) {}

int re_match (struct re_pattern_buffer *buffer, const char *string,
              int length, int start, struct re_registers *regs) {}

int re_match_2 (struct re_pattern_buffer *buffer, const char *string1,
                int length1, const char *string2, int length2,
                int start, struct re_registers *regs, int stop) {}

void re_set_registers (struct re_pattern_buffer *buffer, struct re_registers *regs,
                       unsigned num_regs, regoff_t *starts, regoff_t *ends) {}

// non-re-entrant BSD versions.
// return value is an error message or NULL
char $tainted * re_comp (const char * p) {}

int re_exec (const char * p) {}

int regcomp (regex_t * preg, const char * pattern, int cflags)
{
    __taint_re_pattern_buffer(preg);
}

int regexec (const regex_t * preg, const char * string, size_t nmatch,
             regmatch_t pmatch[], int eflags) {}

size_t regerror (int errcode, const regex_t *preg,
                 char $tainted *errbuf, size_t errbuf_size) {}

void regfree (regex_t *preg) {}
