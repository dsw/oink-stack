#include <glibc-fs/prelude.h>
#include <locale.h>

// struct lconv
// {
//     char *decimal_point;
//     char *thousands_sep;

//     char *grouping;

//     char *int_curr_symbol;
//     char *currency_symbol;
//     char *mon_decimal_point;
//     char *mon_thousands_sep;
//     char *mon_grouping;
//     char *positive_sign;
//     char *negative_sign;
//     char int_frac_digits;
//     char frac_digits;

//     char p_cs_precedes;

//     char p_sep_by_space;

//     char n_cs_precedes;

//     char n_sep_by_space;

//     char p_sign_posn;
//     char n_sign_posn;

//     char int_p_cs_precedes;
//     char int_p_sep_by_space;
//     char int_n_cs_precedes;
//     char int_n_sep_by_space;
//     char int_p_sign_posn;
//     char int_n_sign_posn;
// };

char $tainted *setlocale (int category, const char *locale) {}

// assume lconv contents are untainted
struct lconv *localeconv (void) {}

__locale_t newlocale (int category_mask, const char *locale, __locale_t base) {}

__locale_t duplocale (__locale_t dataset) {}

void freelocale (__locale_t dataset) {}

__locale_t uselocale (__locale_t dataset) {}
