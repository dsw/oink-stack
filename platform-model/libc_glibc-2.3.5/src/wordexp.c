#include <glibc-fs/prelude.h>
#include <wordexp.h>

// typedef struct
// {
//     size_t we_wordc;
//     char **we_wordv;
//     size_t we_offs;
// } wordexp_t;

int wordexp (const char * words, wordexp_t * pwordexp, int flags)
{
    pwordexp->we_wordv[0][0] = words[0];
}

void wordfree (wordexp_t *wordexp) {}
