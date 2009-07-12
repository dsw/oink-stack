/* see License.txt for copyright and terms of use */

#ifndef LIBQUAL_H
#define LIBQUAL_H

/* Call this at any time to reclaim all memory used by libqual and to
   reset libqual to its initial state (i.e., the state when the
   library has just been loaded).  Note: after calling this function,
   you will again need to call init_quals etc. */
void reset_libqual(void);

/* Qualifiers and qualified types */
typedef struct Qtype *qtype;

/* Stores and effects */
typedef struct Abstract_loc *aloc;
typedef struct Effect *effect;
typedef struct Store *store;

/* Polymorphism */
typedef enum {p_neg = -1, p_non = 0, p_pos = 1, p_sub = 2} polarity;
/* jf: p_sub can be used for standard flow */

/* Possible effect constructors */
typedef enum {
  eff_any = 0,
  eff_rwr,
  eff_r,
  eff_wr,
  eff_alloc,
  eff_last = eff_alloc } eff_kind;

/* Other */
typedef int (*printf_func)(const char *fmt, ...);
typedef struct Closure {
  void (*invoke)(struct Closure *);
} *closure;

static inline void invoke_closure(closure c) {
  c->invoke(c);
}

/* Load in a lattice configuration file */
void load_config_file_quals(const char *);

/* jf - shouldn't need stuff past here */
typedef struct Rinfo *rinfo; /* Info for restrict */
typedef struct Drinfo *drinfo; /* Info for deep restrict */
typedef struct ExprDrinfoPair *exprdrinfo; /* Info for deep restrict */

#endif
