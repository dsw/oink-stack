// $Id$

#include <glibc-fs/prelude.h>
#include <rpc/auth_des.h>

// struct authdes_fullname
// {
//     char *name;
//     des_block key;
//     uint32_t window;
// };

// struct authdes_cred
// {
//     enum authdes_namekind adc_namekind;
//     struct authdes_fullname adc_fullname;
//     uint32_t adc_nickname;
// };

// struct rpc_timeval
// {
//     uint32_t tv_sec;
//     uint32_t tv_usec;
// };

// struct authdes_verf
// {
//     union
//     {
// 	struct rpc_timeval adv_ctime;
// 	des_block adv_xtime;
//     }
//     adv_time_u;
//     uint32_t adv_int_u;
// };

int authdes_getucred (const struct authdes_cred * adc, uid_t * uid, gid_t * gid, short *grouplen, gid_t * groups) {}

int getpublickey (const char *name, char $tainted *key) {}

int getsecretkey (const char *name, char $tainted *key, const char *passwd) {}

int rtime (struct sockaddr_in *addrp, struct rpc_timeval *timep, struct rpc_timeval *timeout) {}
