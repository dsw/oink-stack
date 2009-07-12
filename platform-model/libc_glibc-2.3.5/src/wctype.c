#include <glibc-fs/prelude.h>
#include <wctype.h>

int iswalnum (wint_t wc) {}
int iswalpha (wint_t wc) {}
int iswcntrl (wint_t wc) {}
int iswdigit (wint_t wc) {}
int iswgraph (wint_t wc) {}
int iswlower (wint_t wc) {}
int iswprint (wint_t wc) {}
int iswpunct (wint_t wc) {}
int iswspace (wint_t wc) {}
int iswupper (wint_t wc) {}
int iswxdigit (wint_t wc) {}
int iswblank (wint_t wc) {}
wctype_t wctype (const char *property) {}
int iswctype (wint_t wc, wctype_t desc) {}
wint_t $_1 towlower (wint_t $_1 wc) {}
wint_t $_1 towupper (wint_t $_1 wc) {}
wctrans_t wctrans (const char *property) {}
wint_t $_1 towctrans (wint_t $_1 wc, wctrans_t desc) {}

int iswalnum_l (wint_t wc, __locale_t locale) {}
int iswalpha_l (wint_t wc, __locale_t locale) {}
int iswcntrl_l (wint_t wc, __locale_t locale) {}
int iswdigit_l (wint_t wc, __locale_t locale) {}
int iswgraph_l (wint_t wc, __locale_t locale) {}
int iswlower_l (wint_t wc, __locale_t locale) {}
int iswprint_l (wint_t wc, __locale_t locale) {}
int iswpunct_l (wint_t wc, __locale_t locale) {}
int iswspace_l (wint_t wc, __locale_t locale) {}
int iswupper_l (wint_t wc, __locale_t locale) {}
int iswxdigit_l (wint_t wc, __locale_t locale) {}
int iswblank_l (wint_t wc, __locale_t locale) {}

wctype_t wctype_l (const char *property, __locale_t locale) {}

int iswctype_l (wint_t wc, wctype_t desc, __locale_t locale) {}

wint_t $_1_2 towlower_l (wint_t $_1 wc, __locale_t locale) {}

wint_t $_1_2 towupper_l (wint_t $_1 wc, __locale_t locale) {}

wctrans_t wctrans_l (const char *property, __locale_t locale) {}

wint_t $_1_2 towctrans_l (wint_t $_1 wc, wctrans_t desc, __locale_t locale) {}
