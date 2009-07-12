#include <glibc-fs/prelude.h>
#include <crypt.h>

char $tainted *crypt (const char *key, const char *salt) {}

void setkey (const char *key) {}
void encrypt (char $tainted *block, int edflag) {}

// struct crypt_data
// {
//     char keysched[16 * 8];
//     char sb0[32768];
//     char sb1[32768];
//     char sb2[32768];
//     char sb3[32768];

//     char crypt_3_buf[14];
//     char current_salt[2];
//     long int current_saltbits;
//     int  direction, initialized;
// };

static inline void __taint_crypt_data(struct crypt_data* p)
{/*T:H*/
    // make these tainted in case anybody reads them
    __DO_TAINT(p->keysched);
    __DO_TAINT(p->sb0);
    __DO_TAINT(p->sb1);
    __DO_TAINT(p->sb2);
    __DO_TAINT(p->sb3);
    __DO_TAINT(p->crypt_3_buf);
    __DO_TAINT(p->current_salt);
}

char $tainted *crypt_r (const char *key, const char *salt, struct crypt_data * data) {
    __taint_crypt_data(data);
}

void setkey_r (const char *key, struct crypt_data * data) {
    __taint_crypt_data(data);
}

void encrypt_r (char $tainted *block, int edflag, struct crypt_data * data) {
    __taint_crypt_data(data);
}
