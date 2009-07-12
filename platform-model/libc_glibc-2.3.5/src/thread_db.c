#ifndef GLIBCFS_IGNORE_INCOMPLETE

#error "Need to think more about this"

#include <glibc-fs/prelude.h>
#include <thread_db.h>

// typedef struct td_thrinfo
// {
//     td_thragent_t *ti_ta_p;
//     unsigned int ti_user_flags;
//     thread_t ti_tid;
//     char *ti_tls;
//     psaddr_t ti_startfunc;
//     psaddr_t ti_stkbase;
//     long int ti_stksize;
//     psaddr_t ti_ro_area;
//     int ti_ro_size;
//     td_thr_state_e ti_state;
//     unsigned char ti_db_suspended;
//     td_thr_type_e ti_type;
//     intptr_t ti_pc;
//     intptr_t ti_sp;
//     short int ti_flags;
//     int ti_pri;
//     lwpid_t ti_lid;
//     sigset_t ti_sigmask;
//     unsigned char ti_traceme;
//     unsigned char ti_preemptflag;
//     unsigned char ti_pirecflag;
//     sigset_t ti_pending;
//     td_thr_events_t ti_events;
// } td_thrinfo_t;

td_err_e td_init (void) {}

td_err_e td_log (void) {}

const char **td_symbol_list (void) {}

td_err_e td_ta_new (struct ps_prochandle *ps, td_thragent_t **ta) {}

td_err_e td_ta_delete (td_thragent_t *ta) {}

td_err_e td_ta_get_nthreads (const td_thragent_t *ta, int *np) {}

td_err_e td_ta_get_ph (const td_thragent_t *ta, struct ps_prochandle **ph) {}

td_err_e td_ta_map_id2thr (const td_thragent_t *ta, pthread_t pt, td_thrhandle_t *th) {}

td_err_e td_ta_map_lwp2thr (const td_thragent_t *ta, lwpid_t lwpid, td_thrhandle_t *th) {}

td_err_e td_ta_thr_iter (const td_thragent_t *ta,
                         td_thr_iter_f *callback, void *cbdata_p,
                         td_thr_state_e state, int ti_pri,
                         sigset_t *ti_sigmask_p,
                         unsigned int ti_user_flags) {}

td_err_e td_ta_tsd_iter (const td_thragent_t *ta, td_key_iter_f *__ki, void *p) {}

td_err_e td_ta_event_addr (const td_thragent_t *ta, td_event_e event, td_notify_t *ptr) {}

td_err_e td_ta_set_event (const td_thragent_t *ta, td_thr_events_t *event) {}

td_err_e td_ta_clear_event (const td_thragent_t *ta, td_thr_events_t *event) {}

td_err_e td_ta_event_getmsg (const td_thragent_t *ta, td_event_msg_t *msg) {}

td_err_e td_ta_setconcurrency (const td_thragent_t *ta, int level) {}

td_err_e td_ta_enable_stats (const td_thragent_t *ta, int enable) {}

td_err_e td_ta_reset_stats (const td_thragent_t *ta) {}

td_err_e td_ta_get_stats (const td_thragent_t *ta, td_ta_stats_t *statsp) {}

td_err_e td_thr_validate (const td_thrhandle_t *th) {}

td_err_e td_thr_get_info (const td_thrhandle_t *th, td_thrinfo_t *infop) {}

td_err_e td_thr_getfpregs (const td_thrhandle_t *th, prfpregset_t *regset) {}

td_err_e td_thr_getgregs (const td_thrhandle_t *th, prgregset_t gregs) {}

td_err_e td_thr_getxregs (const td_thrhandle_t *th, void *xregs) {}

td_err_e td_thr_getxregsize (const td_thrhandle_t *th, int *sizep) {}

td_err_e td_thr_setfpregs (const td_thrhandle_t *th, const prfpregset_t *fpregs) {}

td_err_e td_thr_setgregs (const td_thrhandle_t *th, prgregset_t gregs) {}

td_err_e td_thr_setxregs (const td_thrhandle_t *th, const void *addr) {}

td_err_e td_thr_tlsbase (const td_thrhandle_t *th, unsigned long int modid, psaddr_t *base) {}

td_err_e td_thr_tls_get_addr (const td_thrhandle_t *th, void *map_address,
                              size_t offset, void **address) {}

td_err_e td_thr_event_enable (const td_thrhandle_t *th, int event) {}

td_err_e td_thr_set_event (const td_thrhandle_t *th, td_thr_events_t *event) {}

td_err_e td_thr_clear_event (const td_thrhandle_t *th, td_thr_events_t *event) {}

td_err_e td_thr_event_getmsg (const td_thrhandle_t *th, td_event_msg_t *msg) {}

td_err_e td_thr_setprio (const td_thrhandle_t *th, int prio) {}

td_err_e td_thr_setsigpending (const td_thrhandle_t *th, unsigned char n, const sigset_t *ss) {}

td_err_e td_thr_sigsetmask (const td_thrhandle_t *th, const sigset_t *ss) {}

td_err_e td_thr_tsd (const td_thrhandle_t *th, const thread_key_t tk, void **data) {}

td_err_e td_thr_dbsuspend (const td_thrhandle_t *th) {}

td_err_e td_thr_dbresume (const td_thrhandle_t *th) {}

#endif
