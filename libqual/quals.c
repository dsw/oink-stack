/* see License.txt for copyright and terms of use */

#if defined(__OPTIMIZE__) && !defined(__NO_INLINE__)
// gcc -O1 or gcc -O2
#  define HASH_EMBED_INLINE 1
#else
// gcc -O0
#  undef HASH_EMBED_INLINE
#endif

#define HASHSET_DEF 1

#include "libqual/hash.h"

#include "libregion/mempool.h"

#include "libqual/location.h" /* shouldn't be here! */
#include "libqual/utils.h"
#include "libqual/quals.h"
#include "libqual/typed_ddlist.h"
#include "libqual/typed_set.h"
#include "libqual/typed_hashset.h"
#include "libqual/typed_map.h"
#include "libqual/hash-serialize.h"
#include "libqual/hashset-serialize.h"
#include "libqual/qual_flags.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// whether we support triggers that are invoked when adding bounds.
#define USE_TRIGGERS 0

// whether we support conditional constraints
#define USE_CONDS 0

// If USE_OWNING_QUAL_EDGE_HASH_TABLES is 1, then use "owner" hash tables for
// u.var.lb and u.var.ub (see hash.c for what owner hash tables are).  The
// advantage of owner hash tables is that we can reclaim rehashed space and
// buckets; entirely unused hash tables can be freed as soon as they are no
// longer needed.  (Otherwise, they can still be freed during compaction.)
// It's faster to use non-owner hash tables, but that leaks memory.
#define USE_OWNING_QUAL_EDGE_HASH_TABLES 1

// Whether to release and reclaim qual_edge hash table memory.  If
// USE_OWNING_QUAL_EDGE_HASH_TABLES, this is done throughout analysis,
// otherwise, only during compaction.
#define RELEASE_QUAL_EDGE_HASH_TABLE_MEMORY 1

// TODO: USE_QUAL_MEMPOOL

// If USE_QUAL_EDGE_MEMPOOL is 1, then use mempools for struct Qual_edge; else
// use a global region.
//
// *** WARNING: This is only for qual_edge objects inside qual_edge_hashsets
//     in a qual!
#define USE_QUAL_EDGE_MEMPOOL 0

// available debugging options:
#define DEBUG_VARS           0
#define DEBUG_EXTERN_QUAL    0
#define DEBUG_HASH_TABLE     0
#define DEBUG_REJECTIONS     0
#define DEBUG_QUAL_GATE      0
#define DEBUG_EDGE_NUMBER    0  /* use edge numbers */
#define DEBUG_SUMMARY_LENGTH 0
#define DEBUG_NEW_SUMMARY    0
#define DEBUG_CONSTRAINT     0

// whether to check edge sets often; very slow.
#define DEBUG_EDGE_SETS      0

// whether to add checks to help valgrind memcheck.
#define DEBUG_VALGRIND       0

//// turns on a subset of the above
// #define DEBUG

#ifdef DEBUG
#define DEBUG_VARS           1
#define DEBUG_REJECTIONS     1
#define DEBUG_QUAL_GATE      1
#define DEBUG_EDGE_NUMBER    1
#define DEBUG_SUMMARY_LENGTH 1
#define DEBUG_EDGE_SETS      1
#endif

#if DEBUG_VALGRIND
# include <valgrind/memcheck.h>
#else
# define NVALGRIND
#endif

// Format: really_check_foo() functions check invariants.
//         DEBUG_CHECK_FOO() macros call really_check_foo() if enabled
//         (usually by DEBUG_FOO)

static void really_check_var(qual q) __attribute__((unused));
static void really_check_vars(qual_set qs) __attribute__((unused));
static void really_check_all_vars() __attribute__((unused));
#if DEBUG_VARS
# define DEBUG_CHECK_VAR(q)     really_check_var(q)
# define DEBUG_CHECK_VARS(qs)   really_check_vars(qs)
# define DEBUG_CHECK_ALL_VARS() really_check_all_vars()
#else
# define DEBUG_CHECK_VAR(q)     ((void)0)
# define DEBUG_CHECK_VARS(qs)   ((void)0)
# define DEBUG_CHECK_ALL_VARS() ((void)0)
#endif

static void really_check_edge_sets(qual_set vars) __attribute__((unused));
void really_check_all_edge_sets(void);
#if DEBUG_EDGE_SETS
# define DEBUG_CHECK_EDGE_SETS(x)     really_check_edge_sets(x)
# define DEBUG_CHECK_ALL_EDGE_SETS(x) really_check_all_edge_sets()
#else
# define DEBUG_CHECK_EDGE_SETS(x)     ((void)0)
# define DEBUG_CHECK_ALL_EDGE_SETS(x) ((void)0)
#endif

static void really_check_summary_length(qual_edge qe) __attribute__((unused));
#if DEBUG_SUMMARY_LENGTH
# define DEBUG_CHECK_SUMMARY_LENGTH(qe) really_check_summary_length(qe)
#else
# define DEBUG_CHECK_SUMMARY_LENGTH(qe) ((void)0)
#endif

#if DEBUG_HASH_TABLE
# define DEBUG_CHECK_HASH_TABLE(ht) hash_table_really_check_for_duplicates((hash_table)ht)
// quarl 2006-06-29
//    This check is no longer relevant as hashsets now only contain keys
// # define DEBUG_CHECK_HASHSET(ht) hash_table_really_check_hashset((hash_table)ht)
# define DEBUG_CHECK_HASHSET(ht) ((void)0)
#else
# define DEBUG_CHECK_HASH_TABLE(ht) ((void)0)
# define DEBUG_CHECK_HASHSET(ht) ((void)0)
#endif

static void really_check_qual_gate(qual_gate *qgate) __attribute__((unused));
static void really_check_all_qual_gates() __attribute__((unused));
#if DEBUG_QUAL_GATE && DEBUG_VALGRIND
# define DEBUG_CHECK_QUAL_GATE(qgate) really_check_qual_gate(qgate)
# define DEBUG_CHECK_ALL_QUAL_GATES() really_check_all_qual_gates()
#else
# define DEBUG_CHECK_QUAL_GATE(qgate) ((void)0)
# define DEBUG_CHECK_ALL_QUAL_GATES() ((void)0)
#endif

#if DEBUG_EXTERN_QUAL
# define DEBUG_SET_EXTERN_QUAL(q,e) really_debug_set_extern_qual(q, e)
# define DEBUG_CHECK_EXTERN_QUAL()  really_check_extern_qual()
#else
# define DEBUG_SET_EXTERN_QUAL(q,e) ((void)0)
# define DEBUG_CHECK_EXTERN_QUAL()  ((void)0)
#endif

#define DEBUG_INIT DEBUG_really_init
static void DEBUG_really_init(void);

#if !defined(DEBUG_INIT)
# define DEBUG_INIT ((void)0)
#endif

/* XXXX:  This shouldn't be here! */
typedef enum {sev_err, sev_warn, sev_info} severity;
void report_qual_error(location loc, severity sev, qual q, const char *format, ...);
/* XXXX */

// statistics
struct Qual_stats qual_stats;
static unsigned long *pquals_unified = NULL;

/**************************************************************************
 *                                                                        *
 * Forward decls                                                          *
 *                                                                        *
 **************************************************************************/

static __inline int cmp_qual(qual left, qual right);
int cmp_qual_byname (qual left, qual right);
static __inline unsigned long hash_qual(qual q);
static __inline int cmp_qual_edge_semantic(qual_edge e1, qual_edge e2);
static __inline unsigned long hash_qual_edge(qual_edge e);
bool eq_qual_edge_exact (qual_edge e1, qual_edge e2);
static inline bool qual_gate_get_qual (qual_gate *qgate, qual q);

static __inline bool eq_qual(qual left, qual right);     /* Returns TRUE iff left and
                                                            right are the same.  Does
                                                            not generate a constraint. */

region quals_s18n_region;

/* For permanent data structures */
static region quals_permanent_region = NULL;

// For quals and qual_edges which are re-allocated in prune_unused_nodes()
static region quals_region = NULL;

#if USE_QUAL_EDGE_MEMPOOL
static struct mempool qual_edges_mempool[1];
static struct mempool *qual_edges_mempool_p = qual_edges_mempool;
#define s18n_qual_edge_allocator mempool_alloc
#define s18n_qual_edge_allocator_data &qual_edges_mempool_p
#else
static region qual_edges_region = NULL;
#define s18n_qual_edge_allocator __rc_ralloc_small0
#define s18n_qual_edge_allocator_data &qual_edges_region
#endif

// qual_edge_hashsets_region should be NULL if we want to use owner hash
// tables.  This is also passed into the serialization functions,
// intentionally as NULL.  We need this variable whether or not
// USE_OWNING_QUAL_EDGE_HASH_TABLES.
#if USE_OWNING_QUAL_EDGE_HASH_TABLES
//   I'd like to make it 'const', but that doesn't work with the serialization
//   macros (and adding a cast there is even worse; too bad we can't do
//   const_cast)
// const
#endif
static region qual_edge_hashsets_region = NULL;

/* For resolving well-formedness conditions */
static region resolve_region = NULL;

/**************************************************************************
 *                                                                        *
 * Data Types                                                             *
 *                                                                        *
 **************************************************************************/

typedef enum { d_bak = -1, d_fwd = 1 } q_direction;

/* An edge in the qualifier graph */
typedef enum { ek_open, ek_flow, ek_close } qedge_kind;

#define LOC_INDEX_BITS 24
#define MAX_LOC_INDEX ((1<<24)-1)
#define INVALID_LOC_INDEX ((unsigned)-1)

struct Qual_edge {
  int summary_time;  /* 0 = not a summary edge */
  qual q1;
  qual q2;
  union {
    // TODO: #ifdef both of these away
    const char *error_message;
    /* This field often overflows, so be careful when you use it. */
    int summary_length;
  } u;

  location loc;

  // put the (potentially) non-word-sized members together at the bottom so
  // that it packs better, and is faster when not word-aligned due to packing.

  /* The gate for this edge */
  qual_gate qgate;

  unsigned loc_index : LOC_INDEX_BITS; /* A unique identifier for each program location */

  qedge_kind kind : 8;

#if DEBUG_EDGE_NUMBER
  int edgeNumber;
#endif
} __attribute__((packed)) ;

// Note: struct Qual_edge is currently the most memory-intensive structure at
// 32 bytes per instance.

// TODO: make summary_time and index 3 bytes each to reduce struct size by 1
// word.

// TODO: when we get around to memory-optimizing on 64-bit machines, create a
// giant array of qual_edges (possibly two-level array), and use indices into
// that array(s).

S18N_GENERIC_STRUCT_BEGIN_A(qedge_basic_type, qedgebas, s18n_qual_edge_allocator, s18n_qual_edge_allocator_data, struct Qual_edge, 4)
  S18N_FIELD(struct Qual_edge, q1, qual_type),
  S18N_FIELD(struct Qual_edge, q2, qual_type),
  S18N_FIELD(struct Qual_edge, loc, location_type),
  S18N_FIELD(struct Qual_edge, u.error_message, s18n_c_string_type)
S18N_GENERIC_STRUCT_END
S18N_GENERIC_STRUCT_BEGIN_A(qedge_summary_type, qedgesum, s18n_qual_edge_allocator, s18n_qual_edge_allocator_data, struct Qual_edge, 3)
  S18N_FIELD(struct Qual_edge, q1, qual_type),
  S18N_FIELD(struct Qual_edge, q2, qual_type),
  S18N_FIELD(struct Qual_edge, loc, location_type),
S18N_GENERIC_STRUCT_END
S18N_ENUM_TAG_TYPE_BEGIN(qual_edge_type, qualedge, struct Qual_edge,
			 summary_time, &qedge_summary_type, 1)
  { 0, &qedge_basic_type }
S18N_ENUM_TAG_TYPE_END

DEFINE_HASHSET(qual_edge_hashset,cmp_qual_edge_semantic,hash_qual_edge,qual_edge);
#define scan_qual_edge_hashset(var, scanner, set) \
for (qual_edge_hashset_scan(set, &scanner), var = qual_edge_hashset_next(&scanner); \
     var; \
     var = qual_edge_hashset_next(&scanner))
DEFINE_HASHSET_S18N(qual_edge_hashset, qual_edge_hashset_type, "qedgehs_",
		    qual_edge_type, qual_edge_hashsets_region/*quals_s18n_region*/, hash_qual_edge);

/* The source of a constant bound on a variable */
struct Qual_reason {
  bool ci_bound : 1;  /*  context-independent bound (the usual type of analysis) */
  enum { rk_none = 0, rk_edge = 1, rk_location = 2 } kind : 2;
  union {
    struct {
      location loc;
      int loc_index;
    } s;
    struct {
      qual_edge qe;
      /* This field often overflows, so be careful when you use it. */
      int pathlength;
    } e;
  } u;

#if DEBUG_EDGE_NUMBER
  int edgeNumber;
#endif
};
typedef struct Qual_reason* qual_reason;

S18N_GENERIC_STRUCT_BEGIN(qreason_none_type, qreason0, struct Qual_reason, 0)
S18N_GENERIC_STRUCT_END
S18N_GENERIC_STRUCT_BEGIN(qreason_edge_type, qreasone, struct Qual_reason, 1)
  S18N_FIELD(struct Qual_reason, u.e.qe, qual_edge_type)
S18N_GENERIC_STRUCT_END
S18N_GENERIC_STRUCT_BEGIN(qreason_location_type, qreasonl, struct Qual_reason, 1)
  S18N_FIELD(struct Qual_reason, u.s.loc, location_type)
S18N_GENERIC_STRUCT_END
S18N_ENUM_TAG_TYPE_BEGIN(qual_reason_type, q_reason, struct Qual_reason, kind, NULL, 3)
  { rk_none, &qreason_none_type },
  { rk_edge, &qreason_edge_type },
  { rk_location, &qreason_location_type }
S18N_ENUM_TAG_TYPE_END

DEFINE_MAP(qual_reason_map,qual,qual_reason,hash_qual,eq_qual);
#define scan_qual_reason_map(kvar, dvar, scanner, map) \
for (qual_reason_map_scan(map, &scanner); \
     qual_reason_map_next(&scanner, &kvar, &dvar);)
DEFINE_MAP_S18N(qreason_map_type, "qreasmap", qual_type, qual_reason_type,
		quals_s18n_region, hash_qual, eq_qual, NULL, NULL, NULL);

/* Return codes for bound insertion.  Should be |'able. */
#define INSERT_NOTHING 0
#define INSERT_LOWER   1
#define INSERT_UPPER   2
#define INSERT_EDGE    4
#define INSERTED_NOTHING(x) ((x) == INSERT_NOTHING)
#define INSERTED_LOWER(x)   (((x) & INSERT_LOWER) != 0)
#define INSERTED_UPPER(x)   (((x) & INSERT_UPPER) != 0)
#define INSERTED_BOTH(x)    (INSERTED_LOWER(x) && INSERTED_UPPER(x))
#define INSERTED_EDGE(x)    (((x) & INSERT_EDGE) != 0)



/* Two incompatible constant bounds on a variable */
typedef struct Qual_error {
  qual lbc;
  qual ubc;
}* qual_error;

DEFINE_LIST(qual_error_list,qual_error);


/* The constant bounds on a variable */
typedef struct Qual_bounds {
  qual_reason_map lbr;
  qual_reason_map ubr;
} *qual_bounds;

S18N_GENERIC_STRUCT_BEGIN(qual_bounds_type, q_bounds, struct Qual_bounds, 2)
     S18N_FIELD(struct Qual_bounds, lbr, qreason_map_type),
     S18N_FIELD(struct Qual_bounds, ubr, qreason_map_type),
S18N_GENERIC_STRUCT_END

typedef struct QInstantiation {
  location loc;
  int loc_index;
  qual q1;
  qual q2;
  polarity p;
} *qinstantiation;

DEFINE_LIST(qinstantiation_list, qinstantiation)



#if USE_CONDS
typedef enum cond_dir { c_leq_v, v_leq_c} cond_dir;

typedef struct Condition {
  cond_dir dir;
  qual c, left, right;         /* constraint is
				   c <= v ==> left <= right   if dir = c_leq_v
				   v <= c ==> left <= right   if dir = v_leq_c
				  where v is the variable this constraint
				  is associated with */
  qual_gate qgate;
  location loc;
  int loc_index;
  bool triggered;              /* TRUE iff this condition has been
				  triggered */
  const char *error_message;
} *cond;

S18N_GENERIC_STRUCT_BEGIN(condition_type, qconditi, struct Condition, 5)
     S18N_FIELD(struct Condition, c, qual_type),
     S18N_FIELD(struct Condition, left, qual_type),
     S18N_FIELD(struct Condition, right, qual_type),
     S18N_FIELD(struct Condition, loc, location_type),
     S18N_FIELD(struct Condition, error_message, s18n_c_string_type)
S18N_GENERIC_STRUCT_END

DEFINE_BAG(cond_set, cond);
#define scan_cond_set(var,scanner,set) \
for (cond_set_scan(set, &scanner), var = cond_set_next(&scanner); \
     var; \
     var = cond_set_next(&scanner))
DEFINE_BAG_S18N(cond_bag_type, "condbag_", condition_type, quals_s18n_region,
		NULL, NULL);

#endif

typedef struct po_info {
  qual_set qualifiers;
  bool flow_sensitive;       /* True if these qualifiers are flow-sensitive */
  bool nonprop;
  bool effect;		     /* True if these qualifiers can propagate over
				effect edges */
  bool casts_preserve;	     /* True if these qualifiers can propagate over
				casts_preserve edges */
} *po_info;


struct Type_qualifier {
  /* Link, for union-find */
  qual link;

  int num_equiv : 29;                 /* Union by rank */
#define MAX_NUM_EQUIV ((1<<29) - 1)
  bool dead : 1;
  enum { qk_constant, qk_variable, qk_link} kind : 2;

  union {

    /* Constant */
    struct {
      const char *name;		     /* Name of this qualifier */
      const char *color;             /* Color information for PAM mode */
      qual_set lbc, ubc;             /* Upper- and lower-bounds in order */
      po_info po;                    /* Partial order this qual is in */
      int gate_index:26;                /* Index in the gate bitset */
      level_qual_t level:3;            /* Level information for qtype */
      sign_qual_t sign:3;              /* Sign information for qtype */
    } elt;

    /* Variable */
    struct {
#if !DELAY_SET_NAMES
      // quarl 2006-06-27
      //    If DELAY_SET_NAMES, then we put use a hashtable in the middle-end
      //    to store names only for those qvars that need names.  Not having
      //    strings is purely a space optimization; not having the name
      //    pointer is a space-for-time trade-off.
      const char *name;
#endif
      location loc;
      int loc_index;
#if USE_TRIGGERS
      closure lb_closure, ub_closure;/* closures that are invoked when
					we add lower/upper bounds */
#endif
      qual_edge_hashset lb, ub;      /* upper/lower bounds (all vars),
					the original constraint graph */
#if USE_CONDS
      cond_set cond_set;             /* conditional constraints pending
					on this variable */
#endif
      qual_bounds qbounds;           /* Constant bounds on this variable */
      bool preferred:1;              /* True if we should keep this name for
					the ecr */
      bool param:1;                  /* True if this var marks a parameter
					of a function */
      bool passed:1;                 /* True if this var marks an instantiated
					parameter of a function. */
      bool returned:1;                 /* True if this var is an instantiated
				        return */
      bool ret:1;                    /* True if this var marks the return of a
					function */
      bool bak_seeded:1;             /* True if this function has already been
					explored (only used for ret nodes) */
      bool fwd_seeded:1;             /* True if this function has already been
					explored (only used for ret nodes) */
      bool bak_fs_seeded:1;          /* True if this function has already been
					explored (only used for ret nodes) */
      bool fwd_fs_seeded:1;          /* True if this function has already been
					explored (only used for ret nodes) */
      bool global:1;                 /* True if this qualifier decorates a
					global variable */
      bool isextern:1;               /* True if this qualifier is on a linker
					visible symbol */
      bool defined:1;                /* True if this extern is fully defined */
      bool visited:1;                /* generic flag for graph traversal */
      bool anonymous:1;              /* Whether this variable was created
					anonymously.  The user probably doesn't
					care to see anonymous variables. */
      bool fwd_summary_dead_end:1;   /* When looking for summaries through this node,
					give up: there are none. */
      bool bak_summary_dead_end:1;
// #if DELAY_SET_NAMES
//       bool needs_name:1; /* whether this qual needs a name */
// #endif
    } var;

  } u;
}; // TODO: pack

#if DELAY_SET_NAMES
# define TQS1 0
#else
# define TQS1 1
#endif
#if USE_CONDS
# define TQS2 1
#else
# define TQS2 0
#endif

// TODO: (de)serialize only the 'lb', since 'ub' can be calculated in a
// post-processing step
S18N_GENERIC_STRUCT_BEGIN(qual_var_type, qualvar_, struct Type_qualifier, (5+TQS1+TQS2))
#if !DELAY_SET_NAMES
     S18N_FIELD(struct Type_qualifier, u.var.name, s18n_c_string_type),
#endif
     S18N_FIELD(struct Type_qualifier, u.var.loc, location_type),
     S18N_FIELD(struct Type_qualifier, link, qual_type),
     S18N_FIELD(struct Type_qualifier, u.var.lb, qual_edge_hashset_type),
     S18N_FIELD(struct Type_qualifier, u.var.ub, qual_edge_hashset_type),
#if USE_CONDS
     S18N_FIELD(struct Type_qualifier, u.var.cond_set, cond_bag_type),
#endif
     S18N_FIELD(struct Type_qualifier, u.var.qbounds, qual_bounds_type)
S18N_GENERIC_STRUCT_END

int qual_serialize_elt(s18n_context *sc, s18n_type *t,
		       void *data, void *args);
int qual_deserialize_elt(s18n_context *sc, s18n_type *t,
			 void * id, void *newdata, void *args);
struct s18n_type_t qual_elt_type =
{
  .name = "qualelt_",
  .serialize = qual_serialize_elt,
  .deserialize = qual_deserialize_elt,
  .register_children = NULL,
  .private_info = NULL
};


S18N_ENUM_TAG_TYPE_BEGIN(qual_type, qualtype, struct Type_qualifier,
			 kind, &qual_elt_type, 2)
  { qk_variable, &qual_var_type },
  { qk_link, &qual_var_type }
S18N_ENUM_TAG_TYPE_END

DEFINE_SET_S18N(qual_set_type, "qualset_", qual_type, quals_s18n_region, NULL, NULL);

DEFINE_HASHSET(qual_hashset,cmp_qual,hash_qual,qual)
#define scan_qual_hashset(var, scanner, set) \
for (qual_hashset_scan(set, &scanner), var = qual_hashset_next(&scanner); \
     var; \
     var = qual_hashset_next(&scanner))


DEFINE_SET(qual_set_byname, qual, cmp_qual_byname)
#define scan_qual_set_byname(var, scanner, set) \
for (qual_set_byname_scan(set, &scanner), var = qual_set_byname_next(&scanner); \
     var; \
     var = qual_set_byname_next(&scanner))


DEFINE_BAG(po_set, po_info);
#define scan_po_set(var,scanner,set) \
for (po_set_scan(set, &scanner), var = po_set_next(&scanner); \
     var; \
     var = po_set_next(&scanner))


DEFINE_LIST(qual_list,qual)


DEFINE_LIST(qual_edge_list, qual_edge)

DEFINE_MAP(qual_edge_pred_map,qual_edge,qual_edge,hash_qual_edge,eq_qual_edge_exact);
#define scan_qual_edge_pred_map(kvar, dvar, scanner, map) \
for (qual_edge_pred_map_scan(map, &scanner); \
     qual_edge_pred_map_next(&scanner, &kvar, &dvar);)

/* Some forward declarations. */
static inline bool qual_gate_set_qual (qual_gate *qgate, qual q, bool allow);
static inline bool qual_gate_passes_qual (qual_gate *qgate, qual q, q_direction d);
static inline bool qual_gate_is_closed (qual_gate *qgate);

/**************************************************************************
 *                                                                        *
 * Global variables                                                       *
 *                                                                        *
 * Note:  If you change initialization here, do it in reset_quals(), too  *
 *                                                                        *
 **************************************************************************/

enum {
  state_start = 0, state_init = 1, state_pos_defined = 2,
  state_finish = 3
} state = state_start;

/* exists_X_qual is set to TRUE if any X qualifiers have been defined in any
   partial orders */
bool exists_effect_qual = FALSE;
bool exists_casts_preserve_qual = FALSE;
bool exists_ptrflow_qual = FALSE;
bool exists_fieldflow_qual = FALSE;
bool exists_fieldptrflow_qual = FALSE;
bool exists_prop_eq_qual = FALSE;

/* used_X_qual is set to TRUE if after end_define_pos() there have been any
   calls to find_qual to look up an X qualifier */
bool used_fs_qual = FALSE;

/* True if polymorphism is enabled */
int flag_poly = 0;

/* All the constant qualifiers */
static qual_set all_quals;

/* For numbering qualifiers uniquely */
static int next_qual = 0;
/* static FILE *graph_file = NULL; */

static po_set all_pos;
static po_info current_po;

static int num_constants = 0;

/* Some common gates */
static qual_gate empty_qgate;       /* no quals */
qual_gate open_qgate;               /* all quals */
qual_gate fi_qgate;                 /* flow-insensitive quals */
qual_gate fs_qgate;                 /* flow-sensitive quals */
qual_gate effect_qgate;             /* effect quals */
qual_gate casts_preserve_qgate;     /* for collapsing below pathological casts */

qual_gate ptrflow_down_pos_qgate;
qual_gate ptrflow_down_neg_qgate;
qual_gate ptrflow_up_pos_qgate;
qual_gate ptrflow_up_neg_qgate;

qual_gate fieldflow_down_pos_qgate;
qual_gate fieldflow_down_neg_qgate;
qual_gate fieldflow_up_pos_qgate;
qual_gate fieldflow_up_neg_qgate;

qual_gate fieldptrflow_down_pos_qgate;
qual_gate fieldptrflow_down_neg_qgate;
qual_gate fieldptrflow_up_pos_qgate;
qual_gate fieldptrflow_up_neg_qgate;

/* Some distinguished qualifiers */
qual const_qual = NULL;
qual nonconst_qual = NULL;
qual volatile_qual = NULL;
qual restrict_qual = NULL;
qual noreturn_qual = NULL; /* __attribute__((noreturn)) */
qual init_qual = NULL;     /* __init */
qual noninit_qual = NULL;

/* Set (externally) to number of hotspots to track. 0 = track no
   hotspots. */
int num_hotspots = 0;

/* Array of size num_hotspots */
qual *hotspots;

/* summaries are time-stamped */
static int current_summary_time = 1;

/* List of all the qinstantiations (for checking well-formedness) */
static qinstantiation_list qinstantiations;

static bool need_cfl_reset = FALSE;

/**** Objects that need to be (de-)serialized ****/

/* All the nodes of the graph */
static qual_set all_vars;

// variables removed from all_vars; keep these around so we can deallocate
// their hashtables.
static qual_set dead_vars;


/**************************************************************************
 *                                                                        *
 * Serialization for lattice elements                                     *
 *                                                                        *
 **************************************************************************/

/* We just serialize the name and then lookup the corresponding element
   on deserialization */

int qual_serialize_elt(s18n_context *sc, s18n_type *t,
		       void *data, void *args)
{
  char eltname[32];
  qual elt = (qual)data;

  assert(strlen(elt->u.elt.name) < sizeof(eltname));
  strncpy(eltname, elt->u.elt.name, sizeof(eltname));
  if (s18n_write(sc, eltname, sizeof(eltname)) != sizeof(eltname))
    return -EIO;

  s18n_record_serialized(sc, data);

  return sizeof(eltname);
}

int qual_deserialize_elt(s18n_context *sc, s18n_type *t,
			 void * id, void *newdata, void *args)
{
  char eltname[32];
  qual elt;

  if (s18n_read(sc, eltname, sizeof(eltname)) != sizeof(eltname))
    return -EIO;

  elt = find_qual(eltname);
  if (elt == NULL || elt->kind != qk_constant)
    return -EIO;

  s18n_record_deserialized(sc, t, id, elt);

  *(qual *)newdata = elt;

  return 0;
}

/**************************************************************************
 *                                                                        *
 * Operations valid on all quals                                          *
 *                                                                        *
 **************************************************************************/

// returns TRUE if this qual is a representative ECR.
static __inline bool is_ecr_qual(qual q)
{
  assert(q);
  return q->link == NULL;
}

/* Return the ECR for this qualifier */
static __inline qual ecr_qual(qual q)
{
  qual ecr, cur, temp;

  assert(q != NULL);

  // quarl 2006-05-22
  //    This function is (manually) loop-unrolled as an optimization, since it
  //    is called a zillion times.

  if (!q) return NULL;

  if (q->link == NULL) {
    // Optimization: q is the ECR; nothing to compress
    return q;
  }

  ecr = q->link;

  /* Find root */
  if (ecr->link == NULL) {
    // Optimization: q->link is the ECR; nothing to compress
    return ecr;
  }

  ecr = ecr->link;

  while (ecr->link)
    ecr = ecr->link;

  /* Compress path */
  cur = q;
  while (cur->link != ecr)
    {
      temp = cur->link;
      cur->link = ecr;
      if (cur->kind == qk_variable) {
        /* quarl: Make sure these are no longer used. */
        assert(cur->u.var.lb);
        assert(cur->u.var.ub);
#if USE_OWNING_QUAL_EDGE_HASH_TABLES && RELEASE_QUAL_EDGE_HASH_TABLE_MEMORY
        destroy_qual_edge_hashset(cur->u.var.lb);
        destroy_qual_edge_hashset(cur->u.var.ub);
#endif
        cur->u.var.lb = NULL;
        cur->u.var.ub = NULL;
      }
      cur = temp;
    }
  return ecr;
}

/* non-lined version of the above */
qual ecr_qual_noninline(qual q)
{
  return ecr_qual(q);
}

bool variable_qual(qual q)
{
  q = ecr_qual(q);
  return q->kind == qk_variable;
}

bool constant_qual(qual q)
{
  q = ecr_qual(q);
  return q->kind == qk_constant;
}

const char *name_qual(qual q)
{
  q = ecr_qual(q);
  switch (q->kind)
    {
    case qk_constant: return q->u.elt.name;
    case qk_variable:
#if DELAY_SET_NAMES
      return qvar_get_name(q);
#else
      return q->u.var.name ? q->u.var.name : "?";
#endif
    default:
      fail("Unexpected kind %d for qualifier\n", q->kind);
    }
}

#if !DELAY_SET_NAMES
bool set_qual_name(qual q, const char *name)
{
  q = ecr_qual(q);
  switch (q->kind)
    {
    case qk_constant:
      fprintf(stderr, "Warning: trying to set name of constant.\n");
      return FALSE;
    case qk_variable: q->u.var.name = name; return TRUE;
    default:
      fail("Unexpected kind %d for qualifier\n", q->kind);
    }

  return FALSE;
}
#endif

/* Return the unique internal name used to identify q (q's address) */
const char *unique_name_qual(qual q)
{
  q = ecr_qual(q);
  return ptr_to_ascii(q);
}

/* A total ordering on qualifiers.  Returns 0 if left = right, a value
   <0 if left < right, or a value >0 if left > right */
static __inline int cmp_qual(qual left, qual right)
{
  left = ecr_qual(left);
  right = ecr_qual(right);
  if (left == right)
    return 0;
  else if (left < right)
    return -1;
  else
    return 1;
}

int cmp_qual_noninline(qual left, qual right)
{
  return cmp_qual(left, right);
}

/* Compare qualifiers by name */
int cmp_qual_byname(qual left, qual right)
{
  /* assert(left->kind == qk_constant && right->kind == qk_constant); */
  int ret;

  left = ecr_qual(left);
  right = ecr_qual(right);

  ret = strcmp(name_qual(left), name_qual(right));
  if (ret)
    return ret;
  else
    return left - right;
}

/* Returns TRUE iff left and right are the same.  Does not generate a
   constraint. */
static __inline bool eq_qual(qual left, qual right)
{
  return (ecr_qual(left) == ecr_qual(right));
}

// move entropy into lower-order bits
static inline unsigned long lcprng(unsigned long v)
{
  v = (v * 1103515245) + 12345;
  return v;
}

/* Hash function on qualifiers */
unsigned long hash_qual(qual q)
{
  q = ecr_qual(q);
  switch (q->kind)
    {
    case qk_constant:
      return string_hash(q->u.elt.name);
    case qk_variable:
      // if (q->u.var.loc) {
      //   unsigned long l = location_index(q->u.var.loc);
      //   if (l != -1)
      //   return l;
      // }
      /* Handle NULL location case */

      return lcprng((unsigned long) q >> 2);
    default:
      fail ("unknown qual kind!\n");
    }
}

/**************************************************************************
 *                                                                        *
 * Partial order elements                                                 *
 *                                                                        *
 **************************************************************************/

void begin_po_qual(void)
{
  if (current_po)
    fail("begin_po_qual called without previous end_po_qual\n");
  current_po = ralloc(quals_permanent_region, struct po_info);
  memset(current_po, 0, sizeof(*current_po));
  current_po->qualifiers = empty_qual_set(quals_permanent_region);
  current_po->flow_sensitive = FALSE;
  current_po->nonprop = FALSE;
  current_po->effect = FALSE;
  current_po->casts_preserve = FALSE;
}

void end_po_qual(void)
{
  if (!current_po)
    fail("end_po_qual called without previous begin_po_qual\n");
  po_set_insert(quals_permanent_region, &all_pos, current_po);
  current_po = NULL;
}

/* Mark the current partial order flow-sensitive */
void set_po_flow_sensitive(void)
{
  current_po->flow_sensitive = TRUE;
  current_po->nonprop = TRUE;
}

/* Mark the current partial order as non-propagating */
void set_po_nonprop(void)
{
  current_po->nonprop = TRUE;
}

/* Mark the current partial order as able to propagate over effect edges */
void set_po_effect(void)
{
  current_po->effect = TRUE;
  exists_effect_qual = TRUE;
}

/* Mark the current partial order as able to propagate over effect edges */
void set_po_casts_preserve(void)
{
  current_po->casts_preserve = TRUE;
  exists_casts_preserve_qual = TRUE;
}

qual add_qual(const char *name)
{
  qual_stats.quals_created++;
  /* qual_set_insert does pointer comparisons, not strcmps, so we need
     to do a special search by name first. */
  qual new_qual;

  assert(state == state_init);
  if (!current_po)
    fail("add_qual called without previous begin_po_qual\n");
  if ((new_qual = find_qual(name)))
    {
      if (new_qual->u.elt.po != current_po)
	fail("Qualifier %s in two different partial orders\n",
	     new_qual->u.elt.name);
      return new_qual;
    }
  /* We didn't find the qualifier */
  /*  printf("Adding qualifier %s\n", name);*/
  // This goes in quals_permanent_region because we can't do anything to
  // compact it
  new_qual = ralloc(quals_permanent_region, struct Type_qualifier);
  memset(new_qual, 0, sizeof(*new_qual));
  // DEBUG_INIT_QUAL(new_qual);
  new_qual->kind = qk_constant;
   /* new_qual->mark = FALSE; */
  new_qual->u.elt.name = rstrdup(quals_permanent_region, name);
  new_qual->u.elt.color = NULL;
  new_qual->u.elt.level = level_value;
  new_qual->u.elt.sign = sign_pos;
  new_qual->u.elt.lbc = empty_qual_set(quals_permanent_region);
  new_qual->u.elt.ubc = empty_qual_set(quals_permanent_region);
  new_qual->u.elt.po = current_po;

  new_qual->u.elt.gate_index = num_constants++;
  if (num_constants > MAX_QUALS) {
    fprintf(stderr, "ERROR: exceeded MAX_QUALS (%d); increase in quals.h\n",
            MAX_QUALS);
    abort();
  }

  new_qual->link = NULL;
  new_qual->num_equiv = 0; // quarl: does it matter whether this is 0 or 1?

  qual_gate_set_qual (&open_qgate, new_qual, TRUE);
  if (current_po->flow_sensitive)
    qual_gate_set_qual (&fs_qgate, new_qual, TRUE);
  else
    qual_gate_set_qual (&fi_qgate, new_qual, TRUE);
  if (current_po->effect)
    qual_gate_set_qual (&effect_qgate, new_qual, TRUE);
  if (current_po->casts_preserve)
    qual_gate_set_qual (&casts_preserve_qgate, new_qual, TRUE);

  qual_set_insert(quals_permanent_region, &all_quals, new_qual);
  qual_set_insert(quals_permanent_region, &current_po->qualifiers, new_qual);

  if (!strcmp(name, "const"))
    const_qual = new_qual;
  else if (!strcmp(name, "$nonconst"))
    nonconst_qual = new_qual;
  else if (!strcmp(name, "volatile"))
    volatile_qual = new_qual;
  else if (!strcmp(name, "restrict"))
    restrict_qual = new_qual;
  else if (!strcmp(name, "noreturn"))
    noreturn_qual = new_qual;
  else if (!strcmp(name, "$init"))
    init_qual = new_qual;
  else if (!strcmp(name, "$noninit"))
    noninit_qual = new_qual;
  return new_qual;
}

void add_qual_lt(qual left, qual right)
{
  assert(left->kind == qk_constant);
  assert(right->kind == qk_constant);
  if (!qual_set_member(left->u.elt.ubc, right))
    {
      qual_set_scanner qs;
      qual q;

      /*
      printf("Adding assumption ");
      print_qual_raw(printf, left);
      printf(" < ");
      print_qual_raw(printf, right);
      printf("\n");
      */

      qual_set_insert(quals_permanent_region, &left->u.elt.ubc, right);
      qual_set_insert(quals_permanent_region, &right->u.elt.lbc, left);

      /* Add transitively-implied assumptions */
      scan_qual_set(q, qs, right->u.elt.ubc)
	add_qual_lt(left, q);

      scan_qual_set(q, qs, left->u.elt.lbc)
	add_qual_lt(q, right);
    }
}

void add_color_qual(qual q, const char *color)
{
  assert(q->kind == qk_constant);
  q->u.elt.color = rstrdup(quals_permanent_region, color);
}

void add_level_qual(qual q, level_qual_t lev)
{
  assert(q->kind == qk_constant);
  assert(lev == level_value || lev == level_ref);

  q->u.elt.level = lev;
}

void add_sign_qual(qual q, sign_qual_t sign)
{
  assert(q->kind == qk_constant);
  assert(sign == sign_pos || sign == sign_neg || sign == sign_eq);
  q->u.elt.sign = sign;
  exists_prop_eq_qual |= (sign == sign_eq && q->u.elt.po->nonprop == FALSE);
}

/* Assert that q flows up or down the ptr type hierarchy. */
void add_ptrflow_qual(qual q, flow_qual_t f)
{
  assert(q->kind == qk_constant);
  switch (f) {
  case flow_up:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&ptrflow_up_pos_qgate, q, TRUE);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&ptrflow_up_neg_qgate, q, TRUE);
    break;
  case flow_down:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&ptrflow_down_pos_qgate, q, TRUE);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&ptrflow_down_neg_qgate, q, TRUE);
    break;
  default:
    fail ("unknown flow direction!\n");
  }
  exists_ptrflow_qual = TRUE;
}

/* Assert that q flows up or down the field type hierarchy. */
void add_fieldflow_qual(qual q, flow_qual_t f)
{
  assert(q->kind == qk_constant);
  switch (f) {
  case flow_up:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldflow_up_pos_qgate, q, TRUE);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldflow_up_neg_qgate, q, TRUE);
    break;
  case flow_down:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldflow_down_pos_qgate, q, TRUE);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldflow_down_neg_qgate, q, TRUE);
    break;
  default:
    fail ("unknown flow direction!\n");
  }
  exists_fieldflow_qual = TRUE;
}

/* Assert that q flows up or down the fieldptr type hierarchy. */
void add_fieldptrflow_qual(qual q, flow_qual_t f)
{
  assert(q->kind == qk_constant);
  switch (f) {
  case flow_up:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldptrflow_up_pos_qgate, q, TRUE);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldptrflow_up_neg_qgate, q, TRUE);
    break;
  case flow_down:
    if (sign_qual (q) == sign_pos || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldptrflow_down_pos_qgate, q, TRUE);
    if (sign_qual (q) == sign_neg || sign_qual(q) == sign_eq)
      qual_gate_set_qual (&fieldptrflow_down_neg_qgate, q, TRUE);
    break;
  default:
    fail ("unknown flow direction!\n");
  }
  exists_fieldptrflow_qual = TRUE;
}

qual_set get_constant_quals (void)
{
  return all_quals;
}

/* Look up a qualifier */
qual find_qual(const char *name)
{
  qual_set_scanner qs;
  qual q;

  scan_qual_set(q, qs, all_quals)
    {
      assert(q->kind == qk_constant);
      if (!strcmp(q->u.elt.name, name))
	{
	  if (state >= state_pos_defined && q->u.elt.po->flow_sensitive)
	    used_fs_qual = TRUE;
	  return q;
	}
    }
  return NULL;
}

/* Change flow-sensitive quals from nonprop to regular quals for this pass */
void reset_flow_sensitive_quals(void)
{
  po_set_scanner ss;
  po_info po;

  scan_po_set(po, ss, all_pos)
    if (po->flow_sensitive)
      po->nonprop = FALSE;
}

level_qual_t level_qual(qual q)
{
  assert(q->kind == qk_constant);
  return q->u.elt.level;
}

sign_qual_t sign_qual(qual q)
{
  assert(q->kind == qk_constant);
  return q->u.elt.sign;
}

/* Return TRUE iff q is flow-sensitive */
bool flow_sensitive_qual(qual q)
{
  assert(q->kind == qk_constant);
  return q->u.elt.po->flow_sensitive;
}

/* Return TRUE iff q is non-propagating */
bool nonprop_qual(qual q)
{
  assert(q->kind == qk_constant);
  return q->u.elt.po->nonprop;
}

/**************************************************************************
 *                                                                        *
 * Qualifier variables                                                    *
 *                                                                        *
 **************************************************************************/

static void really_check_summary_length(qual_edge qe)
{
  assert(!qe->summary_time ||
         (qe->u.summary_length >= 0 &&
          qe->u.summary_length <= 10000));
}

static void really_check_var (qual q)
{
  qual_edge_hashset_scanner qess;
  qual_edge qe;

  q = ecr_qual (q);
  assert (q->kind == qk_variable);

  scan_qual_edge_hashset (qe, qess, q->u.var.lb)
  {
    assert(eq_qual (qe->q2, q));
    DEBUG_CHECK_SUMMARY_LENGTH(qe);
  }

  scan_qual_edge_hashset (qe, qess, q->u.var.ub)
  {
    assert(eq_qual (qe->q1, q));
  }
}

static void really_check_vars (qual_set vars)
{
  qual_set_scanner qss;
  qual q;

  scan_qual_set (q, qss, vars)
  {
    really_check_var(q);
  }
}

static void really_check_all_vars()
{
  really_check_vars(all_vars);
}

qual make_qvar(
#if !DELAY_SET_NAMES
  const char *name,
#endif
  location loc, int loc_index,
  bool preferred, bool global)
{
  // DEBUG_CHECK_ALL_QUAL_GATES();
  qual_stats.quals_created++;
  qual fresh;

  assert(state >= state_pos_defined);
  /* assert(loc!=NO_LOCATION); jfoster -- comment out because in effect.c a qvar will a
      NULL location is created indirectly in qtype_from_store. */

  fresh = ralloc(quals_region, struct Type_qualifier);
  memset(fresh, 0, sizeof(*fresh));
  // DEBUG_INIT_QUAL(fresh);
  fresh->kind = qk_variable;
#if !DELAY_SET_NAMES
  fresh->u.var.name = name;
#endif
#if USE_TRIGGERS
  fresh->u.var.lb_closure = NULL;
  fresh->u.var.ub_closure = NULL;
#endif
#if USE_CONDS
  fresh->u.var.cond_set = NULL;
#endif
  // empty_qual_edge_hashset(NULL) creates an "owning" hashset, meaning the
  // hashset does its own allocation and we need to call
  // destroy_qual_edge_hashset to delete it.  The upshot is that when it
  // reallocates internal data it doesn't leak.  This is important for
  // long-lasting, hash tables such as these.
  fresh->u.var.lb = empty_qual_edge_hashset(qual_edge_hashsets_region);
  fresh->u.var.ub = empty_qual_edge_hashset(qual_edge_hashsets_region);
  fresh->u.var.preferred = preferred;
  fresh->u.var.param = FALSE;
  fresh->u.var.passed = FALSE;
  fresh->u.var.returned = FALSE;
  fresh->u.var.ret = FALSE;
  fresh->u.var.bak_seeded = FALSE;
  fresh->u.var.fwd_seeded = FALSE;
  fresh->u.var.bak_fs_seeded = FALSE;
  fresh->u.var.fwd_fs_seeded = FALSE;
  fresh->u.var.isextern = FALSE;
  fresh->u.var.defined = FALSE;
  fresh->u.var.visited = FALSE;
  fresh->u.var.anonymous = !preferred;  /*  FIXME */
  fresh->u.var.global = global;
  fresh->u.var.loc = loc;
  assert(loc_index <= MAX_LOC_INDEX);
  fresh->u.var.loc_index = loc_index;
  fresh->u.var.qbounds = NULL;
  fresh->u.var.fwd_summary_dead_end = FALSE;
  fresh->u.var.bak_summary_dead_end = FALSE;
  fresh->link = NULL;
  fresh->num_equiv = 1;

  /* Since this variable is fresh, we don't need qual_set_insert() to
     check whether this variable is already in the set.  So use
     qual_set_insert_nocheck() instead.  This gives a major speedup. */
  qual_set_insert_nocheck(quals_permanent_region,&all_vars,fresh);

  // fprintf(stderr, "## make_qvar: inserting %p\n", fresh);

  DEBUG_CHECK_ALL_VARS();
  return fresh;
}

#if DELAY_SET_NAMES
// TODO
#else
qual make_fresh_qvar(const char* base_name, location loc, int loc_index)
{
  const char *name;
  qual q;

  name = rsprintf(quals_permanent_region, "%s#%d", base_name, next_qual++);
  q = make_qvar(name, loc, loc_index, FALSE,FALSE);
  q->u.var.anonymous = TRUE;

  return q;
}
#endif

#if DELAY_SET_NAMES
// TODO: support this... since only do_print_quals_graph() uses it and only to
// get the "root", it doesn't need to be this general though
#else
qual find_var(const char *name)
{
  qual_set_scanner qs;
  qual q;

  scan_qual_set(q, qs, all_vars)
    {
      if (q->kind == qk_variable)
	{
	  if (!strcmp (q->u.var.name, name))
	    return q;
	}
    }

  return NULL;
}
#endif

qual_set get_variable_quals (void)
{
  return all_vars;
}

void set_global_qual (qual q, bool global)
{
  q = ecr_qual (q);
  assert (q->kind == qk_variable);

  assert (state < state_finish);

  q->u.var.global = global;
}

// for debugging
#if DEBUG_EXTERN_QUAL
static qual_hashset extern_quals = NULL;
static region extern_quals_region = NULL;
static void really_debug_set_extern_qual(qual q, bool is_extern)
{
  if (!extern_quals) {
    // extern_quals_region = newregion();
    extern_quals = empty_qual_hashset(extern_quals_region);
  }
  if (is_extern) {
    qual_hashset_insert(&extern_quals, q);
  }
}

static void really_check_extern_qual()
{
  if (extern_quals) {
    qual q;
    qual_hashset_scanner qhss;

    scan_qual_hashset (q, qhss, extern_quals) {
      qual eq = ecr_qual(q);
      // assert (q->kind == qk_variable);
      assert (eq->kind == qk_variable);
      assert (q->u.var.isextern);
      assert (eq->u.var.isextern);
    }
  }
}
#endif

void set_extern_qual (qual q, bool is_extern)
{
  // printf("## set_extern_qual: q=%p(%p), is_extern := %d\n",
  //        q, ecr_qual(q), is_extern);
  q = ecr_qual(q);
  assert(q->kind == qk_variable);
  q->u.var.isextern = is_extern;
  DEBUG_SET_EXTERN_QUAL(q, is_extern);
}

// Set all variable quals to the given externness.
void set_extern_all_qual (bool is_extern)
{
  qual_set_scanner qss;
  qual q;
  scan_qual_set(q, qss, all_vars) {
    if (q->kind == qk_variable) {
      q->u.var.isextern = is_extern;
    }
  }
}

bool extern_qual (qual q)
{
  // printf("## extern_qual: q=%p(%p), is_extern == %d\n",
  //        q, ecr_qual(q), ecr_qual(q)->u.var.isextern);
  q = ecr_qual(q);
  assert(q->kind == qk_variable);
  return q->u.var.isextern;
}


void set_defined_qual (qual q, bool defined)
{
  q = ecr_qual(q);
  assert(q->kind == qk_variable);
  q->u.var.defined = defined;
}

/* TODO Check if qualifier flows into this qual at cast */
void mk_no_qual_qual(location loc, int loc_index, qual q)
{
}

#if USE_TRIGGERS

static bool is_ubounded_by_constant (qual q)
{
  q = ecr_qual (q);
  return q->u.var.qbounds && qual_reason_map_size (q->u.var.qbounds->ubr) > 0;
}

static bool is_lbounded_by_constant (qual q)
{
  q = ecr_qual (q);
  return q->u.var.qbounds && qual_reason_map_size (q->u.var.qbounds->lbr) > 0;
}
/*
static bool is_bounded_by_constant (qual q)
{
  return is_ubounded_by_constant (q) && is_lbounded_by_constant (q);
}
*/

// I don't know if anybody uses these, but it's useful to save 8 bytes from
// struct Type_qualifier if they're not needed...

/* Add a closure c that's invoked whenever a lower bound is added to q */
void add_lb_trigger_qual(qual q, closure c)
{
  q = ecr_qual(q);
  assert(q->kind == qk_variable);
  assert(!q->u.var.lb_closure);
  q->u.var.lb_closure = c;
  if (is_lbounded_by_constant(q))
    invoke_closure(c);
}

/* Add a closure c that's invoked whenever an upper bound is added to q */
void add_ub_trigger_qual(qual q, closure c)
{
  q = ecr_qual(q);
  assert(q->kind == qk_variable);
  assert(!q->u.var.ub_closure);
  q->u.var.ub_closure = c;
  if (is_ubounded_by_constant(q))
    invoke_closure(c);
}
#endif

static __inline bool global_qual(qual q)
{
  q = ecr_qual(q);
  assert(q->kind == qk_variable);
  return q->u.var.global;
}

static bool param_qual(qual q)
{
  q = ecr_qual(q);
  assert (q->kind == qk_variable);
  return q->u.var.param || global_qual(q);
}

static bool passed_qual(qual q)
{
  q = ecr_qual(q);
  assert (q->kind == qk_variable);
  return q->u.var.passed || global_qual(q);
}

static bool ret_qual(qual q)
{
  q = ecr_qual(q);
  assert (q->kind == qk_variable);
  return q->u.var.ret || global_qual(q);
}

static bool returned_qual(qual q)
{
  q = ecr_qual(q);
  assert (q->kind == qk_variable);
  return q->u.var.returned || global_qual(q);
}

bool preferred_qual(qual q)
{
  q = ecr_qual(q);
  assert(q->kind == qk_variable);
  return q->u.var.preferred;
}

// location location_qual(qual q)
// {
//   q = ecr_qual(q);
//   assert(q->kind == qk_variable);
//   return q->u.var.loc;
// }

static qual_edge_set _lb_qual (region r, qual q, bool context_summary)
{
  qual_reason_map_scanner qrms;
  qual c;
  qual_reason qr;
  qual_edge_set result = empty_qual_edge_set(r);
  qual_edge qe;
  qual_edge_hashset_scanner qess;

  q = ecr_qual (q);
  assert (q->kind == qk_variable);
  if (q->u.var.qbounds)
    scan_qual_reason_map(c, qr, qrms, q->u.var.qbounds->lbr)
      if (qr->ci_bound || context_summary)
	{
          // NOTE: this is a temporary qual_edge, so use regions, even if
          // USE_QUAL_EDGE_MEMPOOL is active!
	  qe = ralloc (r, struct Qual_edge);
          memset(qe, 0, sizeof(*qe)); // zero-fill
          // DEBUG_INIT_QUAL_EDGE(qe);
	  qe->q1 = c;
	  qe->q2 = q;
	  qe->kind = ek_flow;
	  qe->loc = qr->kind == rk_location ? qr->u.s.loc : NO_LOCATION;
          qe->loc_index = qr->kind == rk_location ? qr->u.s.loc_index : 0;
          assert(qe->loc_index <= MAX_LOC_INDEX);
	  qual_edge_set_insert (r, &result, qe);
	}

  scan_qual_edge_hashset (qe, qess, q->u.var.lb)
    {
      DEBUG_CHECK_SUMMARY_LENGTH(qe);
      qual_edge_set_insert (r, &result, qe);
    }

  return result;
}

qual_edge_set lb_qual (region r, qual q)
{
  return _lb_qual (r, q, FALSE);
}

qual_edge_set cs_lb_qual (region r, qual q)
{
  return _lb_qual (r, q, TRUE);
}

static qual_edge_set _ub_qual (region r, qual q, bool context_summary)
{
  qual_reason_map_scanner qrms;
  qual c;
  qual_reason qr;
  qual_edge_set result = empty_qual_edge_set(r);
  qual_edge qe;
  qual_edge_hashset_scanner qess;

  q = ecr_qual (q);
  assert (q->kind == qk_variable);
  if (q->u.var.qbounds)
    scan_qual_reason_map(c, qr, qrms, q->u.var.qbounds->ubr)
      if (qr->ci_bound || context_summary)
	{
          // temporary Qual_edge
	  qe = ralloc (r, struct Qual_edge);
          memset(qe, 0, sizeof(*qe)); // zero-fill
          // DEBUG_INIT_QUAL_EDGE(qe);
	  qe->q1 = q;
	  qe->q2 = c;
	  qe->kind = ek_flow;
	  qe->loc = qr->kind == rk_location ? qr->u.s.loc : NO_LOCATION;
          qe->loc_index = qr->kind == rk_location ? qr->u.s.loc_index : 0;
          assert(qe->loc_index <= MAX_LOC_INDEX);
	  qual_edge_set_insert (r, &result, qe);
	}

  scan_qual_edge_hashset (qe, qess, q->u.var.ub)
    {
      DEBUG_CHECK_SUMMARY_LENGTH(qe);
      qual_edge_set_insert (r, &result, qe);
    }

  return result;
}

qual_edge_set ub_qual (region r, qual q)
{
  return _ub_qual (r, q, FALSE);
}

qual_edge_set cs_ub_qual (region r, qual q)
{
  return _ub_qual (r, q, TRUE);
}

int error_count_qual(qual q)
{
#if 0
   /*  Only used for hotspots, which are deprecated. */
  q = ecr_qual(q);
  assert(q->kind == qk_variable);
  return q->u.var.num_errors;
#else
  return 0;
#endif
}

/* Whether a qualifier is an effect qualifier */
bool effect_qual(qual q)
{
  q = ecr_qual(q);
  assert(q->kind == qk_constant);
  return q->u.elt.po->effect;
}

/* Return TRUE iff q has bound as an originally-specified lower-bound */
static bool _has_lb_qual(qual q, qual bound, bool context_summary)
{
  qual_reason qr;
  bool result = FALSE;

  q = ecr_qual (q);
  assert (q->kind == qk_variable);

  if (bound == NULL)
    {
      qual_reason_map_scanner qrms;
      qual t;

      if (q->u.var.qbounds)
	scan_qual_reason_map(t, qr, qrms, q->u.var.qbounds->lbr)
	  if (qr->kind == rk_location)
	    {
	      result = TRUE;
	      break;
	    }
    }
  else if (constant_qual(bound))
    {
      if (q->u.var.qbounds && qual_reason_map_lookup(q->u.var.qbounds->lbr, bound, &qr))
	result = qr->kind == rk_location || result;
    }
  else
    {
      qual_edge_set_scanner qess;
      qual_edge qe;
      region scratch = newregion();

      scan_qual_edge_set(qe, qess, _lb_qual(scratch, q, context_summary))
	{
	  if (qe->kind == ek_flow)
	    {
	      if (eq_qual(bound, qe->q1))
		{
		  result = TRUE;
		  break;
		}
	    }
	  DEBUG_CHECK_SUMMARY_LENGTH(qe);
	}

      deleteregion (scratch);
    }

  return result;
}

bool has_lb_qual(qual q, qual bound)
{
  return _has_lb_qual (q, bound, FALSE);
}

bool cs_has_lb_qual(qual q, qual bound)
{
  return _has_lb_qual (q, bound, TRUE);
}

/* Return TRUE iff q has bound as an originally-specified upper-bound */
static bool _has_ub_qual(qual q, qual bound, bool context_summary)
{
  qual_reason qr;
  bool result = FALSE;

  q = ecr_qual (q);
  assert (q->kind == qk_variable);

  if (bound == NULL)
    {
      qual_reason_map_scanner qrms;
      qual t;

      if (q->u.var.qbounds)
	scan_qual_reason_map(t, qr, qrms, q->u.var.qbounds->ubr)
	  if (qr->kind == rk_location)
	    {
	      result = TRUE;
	      break;
	    }
    }
  else if (constant_qual(bound))
    {
      if (q->u.var.qbounds && qual_reason_map_lookup(q->u.var.qbounds->ubr, bound, &qr))
	result =  qr->kind == rk_location || result;
    }
  else
    {
      qual_edge_set_scanner qess;
      qual_edge qe;
      region scratch = newregion();

      scan_qual_edge_set(qe, qess, _ub_qual(scratch, q, context_summary))
	{
	  if (qe->kind == ek_flow)
	    {
	      if (eq_qual(bound, qe->q2))
		{
		  result = TRUE;
		  break;
		}
	    }
	  DEBUG_CHECK_SUMMARY_LENGTH(qe);
	}

      deleteregion (scratch);
    }

  return result;
}

bool has_ub_qual(qual q, qual bound)
{
  return _has_ub_qual (q, bound, FALSE);
}

bool cs_has_ub_qual(qual q, qual bound)
{
  return _has_ub_qual (q, bound, TRUE);
}

/* Return TRUE iff q has bound as an originally-specified bound */
bool has_qual(qual q, qual bound)
{
  return has_lb_qual(q, bound) || has_ub_qual(q, bound);
}

bool cs_has_qual(qual q, qual bound)
{
  return cs_has_lb_qual(q, bound) || cs_has_ub_qual(q, bound);
}

/* Return TRUE iff q has any flow-sensitive qualifier as an
   originally-specified bound. */
bool has_fs_qual(qual q)
{
  qual qc;
  qual_reason qr;
  qual_reason_map_scanner qrms;

  q = ecr_qual (q);
  if (! q->u.var.qbounds)
    return FALSE;

  scan_qual_reason_map (qc, qr, qrms, q->u.var.qbounds->lbr)
    if ((qr->kind == rk_location || qr->kind == rk_none) &&
	qc->u.elt.po->flow_sensitive)
      return TRUE;

  scan_qual_reason_map (qc, qr, qrms, q->u.var.qbounds->ubr)
    if ((qr->kind == rk_location || qr->kind == rk_none) &&
	qc->u.elt.po->flow_sensitive)
      return TRUE;

  return FALSE;
}

/**************************************************************************
 *                                                                        *
 * Queries on the ordering                                                *
 *                                                                        *
 **************************************************************************/

/* Return TRUE iff the constraints imply q1 <= q2, where at least one
   of q1 and q2 is a constant. This function can only be called after
   finish_quals(). */
static bool _leq_qual (qual q1, qual q2, bool context_summary)
{
  qual_reason qr;

  assert (state == state_finish);

  q1 = ecr_qual (q1);
  q2 = ecr_qual (q2);

  if (eq_qual (q1, q2))
    return TRUE;

  if (constant_qual(q1) && variable_qual(q2))
    {
      qual c;
      qual_reason_map_scanner qrms;

      if (!q2->u.var.qbounds)
	return FALSE;

      scan_qual_reason_map (c, qr, qrms, q2->u.var.qbounds->lbr)
	if (qr->ci_bound || context_summary)
	  if (leq_qual (q1, c))
	    return TRUE;

      return FALSE;
    }
  else if (variable_qual(q1) && constant_qual(q2))
    {
      qual c;
      qual_reason_map_scanner qrms;

      if (!q1->u.var.qbounds)
	return FALSE;

      scan_qual_reason_map (c, qr, qrms, q1->u.var.qbounds->ubr)
	if (qr->ci_bound || context_summary)
	  if (leq_qual (c, q2))
	    return TRUE;

      return FALSE;
     }
  else if (constant_qual(q1) && constant_qual(q2))
    {
      return qual_set_member (q1->u.elt.ubc, q2);
    }
  else
    {
      /* Should never happen */
      assert (0);
    }

  /* Should never happen */
  return FALSE;
}

bool leq_qual (qual q1, qual q2)
{
  return _leq_qual (q1, q2, FALSE);
}

bool cs_leq_qual (qual q1, qual q2)
{
  return _leq_qual (q1, q2, TRUE);
}

int qual_error_cmp_fn(const qual_error qe1, const qual_error qe2)
{
  int r = strcmp(qe1->lbc->u.elt.name, qe2->lbc->u.elt.name);
  if (r)
    return r;
  return strcmp(qe1->ubc->u.elt.name, qe2->ubc->u.elt.name);
}

/* Returns TRUE iff left <= right is consistent with current
   system of constraints.  By defn, returns TRUE if left and right are
   in distinct partial orders.

   If error_list is non-NULL, then each contradictory pair of bounds will be
   inserted into error_list using region r. */
static bool _may_leq_qual(qual left, qual right, bool context_summary,
                          qual_error_list* error_list, region r)
{
  qual_reason_map_scanner scan_lb, scan_ub;
  qual lbc, ubc;
  qual_reason lbr, ubr;
  bool retval = TRUE;

  left = ecr_qual(left);
  right = ecr_qual(right);

  /* Comparing two constants: they are leq if they are in separate
     po's or left <= right in the poset. */
  if (constant_qual(left) && constant_qual(right))
    {
      assert (state >= state_pos_defined);

      if (left == right)
	return TRUE;

      return (qual_set_member(left->u.elt.ubc, right) ||
	      !qual_set_member(left->u.elt.po->qualifiers, right));
    }

  /* var <= const. This is true if every lower bound of var is <=
     const */
  else if (variable_qual(left) && constant_qual(right))
    {
       /* assert (state >= state_finish); */

      if (! left->u.var.qbounds)
	return TRUE;

      scan_qual_reason_map(lbc,lbr,scan_lb,left->u.var.qbounds->lbr)
	if (lbr->ci_bound || context_summary)
	  if (! may_leq_qual (lbc, right))
	    return FALSE;

      return TRUE;
    }

  /* const <= var. This is true if every upper bound of var is >=
     const. */
  else if (constant_qual(left) && variable_qual(right))
    {
       /* assert (state >= state_finish); */

      if (! right->u.var.qbounds)
	return TRUE;

      scan_qual_reason_map(ubc,ubr,scan_ub,right->u.var.qbounds->ubr)
	if (ubr->ci_bound || context_summary)
	  if (! may_leq_qual (left, ubc))
	    return FALSE;

      return TRUE;
    }

  /* var1 <= var2. This is consistent if every lower bound of var1 is
     <= every upper bound of var2 */
   /* assert (state >= state_finish); */

  if (! left->u.var.qbounds)
    return TRUE;
  if (! right->u.var.qbounds)
    return TRUE;

  scan_qual_reason_map(lbc,lbr,scan_lb,left->u.var.qbounds->lbr)
    if (lbr->ci_bound || context_summary)
      scan_qual_reason_map(ubc,ubr,scan_ub,right->u.var.qbounds->ubr)
	if (ubr->ci_bound || context_summary)
	  {
	    assert (constant_qual(lbc) && constant_qual(ubc));
	    if (! may_leq_qual (lbc, ubc))
	      {
#ifdef DEBUG
		print_qual_raw(printf,lbc, &open_qgate);
		printf(" >= ");
		print_qual_raw(printf,ubc, &open_qgate);
		putchar('\n');
#endif
// #if DELAY_SET_NAMES
//                 lbc->u.var.needs_name = TRUE;
//                 ubc->u.var.needs_name = TRUE;
// #endif
		if (error_list != NULL)
		  {
		    qual_error qe = ralloc (r, struct Qual_error);
                    memset(qe, 0, sizeof(*qe));
		    qe->lbc = lbc;
		    qe->ubc = ubc;
		    *error_list = qual_error_list_cons (r, *error_list, qe);

		    retval = FALSE;
		  }
		else
		  return FALSE;
	      }
	  }

  if (error_list)
    qual_error_list_sort(*error_list, qual_error_cmp_fn);

  return retval;
}

/* Returns TRUE iff left <= right is consistent with current
   system of constraints.  By defn, returns TRUE if left and right are
   in distinct partial orders. */
bool may_leq_qual(qual left, qual right)
{
  return _may_leq_qual(left, right, FALSE, NULL, NULL);
}

bool cs_may_leq_qual(qual left, qual right)
{
  return _may_leq_qual(left, right, TRUE, NULL, NULL);
}

static void reset_visited (qual_set vars)
{
  qual_set_scanner qss;
  qual q;

  scan_qual_set (q, qss, vars)
    if (q->kind == qk_variable)
      q->u.var.visited = FALSE;
}

/**************************************************************************
 *                                                                        *
 * Print/View Qualifiers                                                  *
 *                                                                        *
 **************************************************************************/

/* Print the raw qualifiers */
int print_qual_raw(printf_func pf, qual q, qual_gate *qgate)
{
  if (!q)
    fail("Null qualifier in print_qual_raw\n");
  if (q->link)
    return print_qual_raw(pf, q->link, qgate);
  switch(q->kind)
    {
    case qk_constant: return pf("%s", q->u.elt.name);
    case qk_variable:
#if DELAY_SET_NAMES
      return pf("%s", qvar_get_name(q));
#else
      return pf("%s", q->u.var.name);
#endif
    default: fail("Unexpected kind %d for qualifier\n", q->kind);
    }
}

/* TODO Print the qualifiers, nicely */
static int _print_qual(printf_func pf, qual q, qual_gate *qgate, bool context_summary)
{
  q = ecr_qual(q);

  switch (q->kind)
    {
    case qk_constant:
      return print_qual_raw(pf, q, qgate);
    case qk_variable:
      if (q->u.var.qbounds)
	{
	  qual_reason_map_scanner qrms;
	  qual_reason qr;
	  qual qc;
	  region scratch;
	  qual_set_byname bounds;
	  qual_set_scanner qss;
	  int printed = 0;
	  bool first = TRUE;

	  scratch = newregion();

	  bounds = empty_qual_set(scratch);

	  scan_qual_reason_map (qc, qr, qrms, q->u.var.qbounds->lbr)
	    if ((qr->ci_bound || context_summary) &&
		(sign_qual(qc) == sign_pos || sign_qual(qc) == sign_eq) &&
		qual_gate_get_qual(qgate, qc))
	      qual_set_byname_insert (scratch, &bounds, qc);
	  scan_qual_reason_map (qc, qr, qrms, q->u.var.qbounds->ubr)
	    if ((qr->ci_bound || context_summary) &&
		sign_qual(qc) == sign_neg &&
		qual_gate_get_qual(qgate, qc))
	      qual_set_byname_insert (scratch, &bounds, qc);

	  qual_set_byname_remove_dups (cmp_qual_byname, bounds);
	  qual_set_byname_sort (bounds);

	  scan_qual_set_byname (qc, qss, bounds)
	    {
	      if (!first)
		printed += pf (" ");
	      printed += print_qual_raw (pf, qc, qgate);
	      first = FALSE;
	    }

	  deleteregion (scratch);
	  return printed;
	}
      return 0;
    default:
      assert (0);
    }

  return 0;
}

int print_qual(printf_func pf, qual q, qual_gate *qgate)
{
  return _print_qual (pf, q, qgate, FALSE);
}

int cs_print_qual(printf_func pf, qual q, qual_gate *qgate)
{
  return _print_qual (pf, q, qgate, TRUE);
}

/* Apply f to all possible values of q.  f is applied to NULL in
   the error case. */
void scan_qual_bounds(qual q, qual_gate *qgate, qual_traverse_fn f, void *arg)
{
  qual_reason_map_scanner qrms;
  qual p;
  qual_reason qr;

  q = ecr_qual(q);
  assert(q->kind == qk_variable);

  if (! q->u.var.qbounds)
     return;

  scan_qual_reason_map(p, qr, qrms, q->u.var.qbounds->lbr)
    if (((sign_qual(p) == sign_pos) || (sign_qual(p) == sign_eq)) &&
	qual_gate_get_qual(qgate, p))
      f(p, arg);

  scan_qual_reason_map(p, qr, qrms, q->u.var.qbounds->ubr)
    if (sign_qual(p) == sign_neg &&      /* eq handled above */
	qual_gate_get_qual(qgate, p))
      f(p, arg);

  /* Do nothing for nonprop qualifiers */
}

struct add_info
{
  region r;
  qual_set_byname *b;
};

/* Add q to bounds */
static void add_bound(qual q, struct add_info *ai)
{
  qual_set_byname_insert(ai->r, ai->b, q);
}

/* Apply f to all possible values of q in sorted order. */
void scan_qual_bounds_sorted(qual q, qual_gate *qgate, qual_traverse_fn f, void *arg)
{
  region scratch_region;
  qual_set_byname bounds;
  qual_set_byname_scanner qsbn;
  struct add_info ai;
  qual p;

  scratch_region = newregion();
  ai.r = scratch_region;
  ai.b = &bounds;
  bounds = empty_qual_set_byname(ai.r);
  scan_qual_bounds(q, qgate, (qual_traverse_fn) add_bound, &ai);
  qual_set_byname_sort(bounds);
  scan_qual_set_byname(p, qsbn, bounds)
    f(p, arg);
  deleteregion(scratch_region);
}


#define ERROR_BOUND ((qual)-1)

static void color_bound_qual_fn(qual q, void *arg)
{
  qual *bound = (qual*)arg;
  po_set_scanner pss;
  po_info po;

  if (*bound == ERROR_BOUND)
    return;

  if (*bound == NULL)
    {
      *bound = q;
      return;
    }

  if (q->u.elt.po == (*bound)->u.elt.po)
    {
      *bound = ERROR_BOUND;
      return;
    }

  scan_po_set(po, pss, all_pos)
    {
      if (po == q->u.elt.po)
	{
	  *bound = q;
	  return;
	}
      if (po == (*bound)->u.elt.po)
	return;
    }
}

/* Return the constant bound on q that should determine
   the color of q, or NULL if no bounds on q, or
   -1 if q has conflicting bounds. */
qual color_bound_qual(qual q, qual_gate *qgate)
{
  q = ecr_qual(q);
  switch (q->kind)
    {
    case qk_constant:
      return q;
    case qk_variable:
      {
	qual bound = NULL;
	scan_qual_bounds(q, qgate, color_bound_qual_fn, &bound);
	return bound;
      }
    default:
      fail("Unexpected kind %d for qualifier\n", q->kind);
    }
}

/* Return the color of qualifier q.  q may be a constant or variable. */
const char *color_qual(qual q, qual_gate *qgate)
{
  qual cb;
  if (q == NULL)
    return NULL;
  else if (q == ERROR_BOUND)
    fail("Use error_qual() to determine if a qualifier has an error");

  cb = color_bound_qual(q, qgate);
  if (cb == NULL)
    return NULL;
  else if (cb == ERROR_BOUND)
    fail("Use error_qual() to determine if a qualifier has an error");
  else
    return cb->u.elt.color;
}

/* Return TRUE iff this qualifier represents an error.  Will always
return FALSE for a constant. */
bool error_qual(qual q, qual_gate *qgate)
{
  qual cb;
  if (q == NULL)
    return FALSE;
  else if (q == ERROR_BOUND)
    return TRUE;

  cb = color_bound_qual(q, qgate);
  return (cb == ERROR_BOUND);
}

/* Given the color bounds q1 and q2, on different levels of a qtype,
   choose the color bound for the whole qtype. */
qual color_bound_qual_qtype_combine(qual q1, qual q2)
{
  if (q1 == NULL)
    return q2;
  if (q2 == NULL)
    return q1;
  if (q1 == ERROR_BOUND)
    return q1;
  if (q2 == ERROR_BOUND)
    return q2;

  assert (constant_qual(q1) && constant_qual(q2));

  if (q1->u.elt.po == q2->u.elt.po)
    {
      qual_set_scanner qss;
      qual po_elt;

      scan_qual_set(po_elt, qss, q1->u.elt.po->qualifiers)
	{
	  if (eq_qual(po_elt, q1))
	    return q1;
	  if (eq_qual(po_elt, q2))
	    return q2;
	}
      fail("Qualifiers not in their own po???");
    }
  else
    {
      po_set_scanner pss;
      po_info po;

      scan_po_set(po, pss, all_pos)
	{
	  if (po == q1->u.elt.po)
	    return q1;
	  if (po == q2->u.elt.po)
	    return q2;
	}
      fail("Qualifiers not in any po???");
    }
}

/**************************************************************************
 *                                                                        *
 * Qual edges                                                             *
 *                                                                        *
 **************************************************************************/

static void really_check_qual_gate(qual_gate *qgate)
{
#if DEBUG_VALGRIND
  VALGRIND_CHECK_READABLE(qgate, sizeof(*qgate));
#endif
}

static void really_check_all_qual_gates()
{
  qual q;
  qual_set_scanner qss;

  scan_qual_set(q, qss, all_vars)
  {
    if (!is_ecr_qual(q)) continue;

    qual_edge_hashset_scanner qess;
    qual_edge e;

    scan_qual_edge_hashset(e, qess, q->u.var.lb) {
      really_check_qual_gate(&e->qgate);
    }
  }
}

#if GATE_WORDS_PACK_FLAGS

// return non-0 if bit is set
#define BIT_GET(w,b) ( (w) & (1<<(b)) )

// set 1 bit
#define BIT_SET(w,b,v) ((w) = (((w) & (~(1<<(b)))) | ((v)?(1<<b):0)))
// #define BIT_SET(w,b,v) ((v) ? ((w) |= (1<<(b))) : ((w) &= ~(1<<(b))))

// // set two consecutive bits at once; input must be one of 00,01,10,11
// #define BIT_SET2(w,b,v) (v = (v & (~(3<<(b))) | ((v) << b)))

// #define BIT_SET2_from_1(w,b,v) ((v) ? ((w) |= (1<<(b))) : ((w) &= ~(1<<(b))))

#define qual_gate_get_forward(qgate) (BIT_GET((qgate)->mask[GATE_FLAGWORD_INDEX], GATE_BIT_FORWARD))
#define qual_gate_set_forward(qgate,v) BIT_SET((qgate)->mask[GATE_FLAGWORD_INDEX], GATE_BIT_FORWARD, v)

#define qual_gate_get_backward(qgate) (BIT_GET((qgate)->mask[GATE_FLAGWORD_INDEX], GATE_BIT_BACKWARD))
#define qual_gate_set_backward(qgate,v) BIT_SET((qgate)->mask[GATE_FLAGWORD_INDEX], GATE_BIT_BACKWARD, v)

#else

#define qual_gate_get_forward(qgate) ((qgate)->forward)
#define qual_gate_set_forward(qgate, v) ((qgate)->forward = v)

#define qual_gate_get_backward(qgate) ((qgate)->backward)
#define qual_gate_set_backward(qgate, v) ((qgate)->backward = v)

// #define qual_gate_set_flags(qgate, vf, vb) do { (qgate)->forward = vf; (qgate)->backward = vb; } while (0)

#endif

static inline
void qual_gate_init (qual_gate * qgate, bool backward, bool forward)
{
  // memset entire data structure to make sure the unused bits are initialized
  // to 0 (otherwise memcmp fails).
  memset (qgate, 0, sizeof(*qgate));
  // memset (qgate->mask, 0, sizeof (qgate->mask));
  qual_gate_set_forward(qgate, forward);
  qual_gate_set_backward(qgate, backward);
  // TODO: use optimized two-bit-setting macros.
  DEBUG_CHECK_QUAL_GATE(qgate);
}

/* Low-low-level gate stuff */
static inline
bool qual_gate_get (gate_word *mask, unsigned int bitindex)
{
  int index;
  int getmask;
  index = bitindex / BITS_PER_GATE_WORD;
  getmask = 1 << (bitindex % BITS_PER_GATE_WORD);
  assert (index < num_constants && index < MAX_QUALS);
  return (mask[index] & getmask) != 0;
}

static inline
bool qual_gate_set (gate_word *mask, unsigned int bitindex, bool val)
{
  int index;
  int setmask;
  bool old;
  index = bitindex / BITS_PER_GATE_WORD;
  setmask = 1 << (bitindex % BITS_PER_GATE_WORD);
  assert (index < num_constants && index < MAX_QUALS);
  old = (mask[index] & setmask) != 0;
  if (val)
    mask[index] |= setmask;
  else
    mask[index] &= (~setmask);
  return old;
}

/* Low-level stuff. */
static inline
void qual_gate_set_all (gate_word *mask, bool val)
{
  gate_word newval = val ? -1 : 0;
  int i;
  for (i = 0; i < GATE_WORDS_FULL; i++)
    mask[i] = newval & open_qgate.mask[i];
#if GATE_WORDS_PACK_FLAGS
  assert (i == GATE_FLAGWORD_INDEX);
  i = GATE_FLAGWORD_INDEX;
  gate_word flags = (mask[i] & GATE_FLAGWORD_FLAG_MASK);
  mask[i] = newval & open_qgate.mask[i] & GATE_FLAGWORD_NOFLAG_MASK;
  mask[i] = mask[i] | flags;
#endif
}

/* returns TRUE if any gate was set */
static inline
bool qual_gate_and (gate_word *a, gate_word *b, gate_word *r)
{
  int i;
  bool rval = FALSE;
  for (i = 0; i < GATE_WORDS_FULL; i++)
    {
      r[i] = a[i] & b[i] & open_qgate.mask[i];
      rval = rval || (r[i] != 0);
    }
#if GATE_WORDS_PACK_FLAGS
  assert (i == GATE_FLAGWORD_INDEX);
  i = GATE_FLAGWORD_INDEX;
  gate_word flags = (r[i] & GATE_FLAGWORD_FLAG_MASK);
  r[i] = a[i] & b[i] & open_qgate.mask[i] & GATE_FLAGWORD_NOFLAG_MASK;
  rval = rval || (r[i] != 0);
  r[i] = r[i] | flags;
#endif
  return rval;
}

static inline
void qual_gate_or (gate_word *a, gate_word *b, gate_word *r)
{
  int i;
  for (i = 0; i < GATE_WORDS_FULL; i++)
    {
      r[i] = (a[i] | b[i]) & open_qgate.mask[i];
    }
#if GATE_WORDS_PACK_FLAGS
  assert (i == GATE_FLAGWORD_INDEX);
  i = GATE_FLAGWORD_INDEX;
  gate_word flags = (r[i] & GATE_FLAGWORD_FLAG_MASK);
  r[i] = (a[i] | b[i]) & open_qgate.mask[i] & GATE_FLAGWORD_NOFLAG_MASK;
  r[i] = r[i] | flags;
#endif
}

/* High-level stuff. */
static inline
bool qual_gate_set_qual (qual_gate *qgate, qual q, bool allow)
{
  q = ecr_qual (q);
  assert (q->kind == qk_constant);
  return qual_gate_set (qgate->mask, q->u.elt.gate_index, allow);
}

// static void qual_gate_set_qual_set (qual_gate *qgate, qual_set qs, bool allow)
// {
//   qual q;
//   qual_set_scanner qss;
//   if (qs)
//     scan_qual_set (q, qss, qs)
//       qual_gate_set_qual (qgate, q, allow);
//   else
//     qual_gate_set_all (qgate->mask, allow);
// }

// static void qual_gate_set_po (qual_gate *qgate, po_info po, bool allow)
// {
//   qual_gate_set_qual_set(qgate, po->qualifiers, allow);
// }

// void qual_gate_set_qual_po (qual_gate *qgate, qual q, bool allow)
// {
//   q = ecr_qual (q);
//   assert (q->kind == qk_constant);
//   qual_gate_set_po (qgate, q->u.elt.po, allow);
// }

static inline
bool qual_gate_get_qual (qual_gate *qgate, qual q)
{
  q = ecr_qual (q);
  assert (q->kind == qk_constant);
  return qual_gate_get (qgate->mask, q->u.elt.gate_index);
}

static inline
bool qual_gate_passes_qual (qual_gate *qgate, qual q, q_direction d)
{
  switch (d)
    {
    case d_fwd:
      if (!qual_gate_get_forward(qgate))
	return FALSE;
      break;
    case d_bak:
      if (!qual_gate_get_backward(qgate))
	return FALSE;
      break;
    default:
      fail ("Unknown direction in qual_gate_passes\n");
    }
  return qual_gate_get_qual (qgate, q);
}

#if 0 /* Not currently used */
static bool qual_gate_passes_set (qual_gate *qgate, qual_set qs, q_direction d)
{
  qual_set_scanner qss;
  qual q;

  scan_qual_set (q, qss, qs)
    if (!qual_gate_passes_qual (qgate, q, d))
      return FALSE;
  return TRUE;
}
#endif

static inline
bool qual_gate_is_closed (qual_gate *qgate)
{
  int i;

  if (!(qual_gate_get_forward(qgate) ||
        qual_gate_get_backward(qgate)))
    return TRUE;

  for (i = 0; i < GATE_WORDS_FULL; i++)
    if (qgate->mask[i] != 0)
      return FALSE;

#if GATE_WORDS_PACK_FLAGS
  assert (i == GATE_FLAGWORD_INDEX);
  i = GATE_FLAGWORD_INDEX;
  if ((qgate->mask[i] & GATE_FLAGWORD_NOFLAG_MASK) != 0)
    return FALSE;
#endif

  return TRUE;
}

// returns TRUE if new gate is not empty
static inline
bool compose_qual_gates (qual_gate *a, qual_gate *b, qual_gate *r)
{
  bool forward = qual_gate_get_forward(a) && qual_gate_get_forward(b);
  bool backward = qual_gate_get_backward(a) && qual_gate_get_backward(b);

  if (! (forward || backward))
    return FALSE;

  // NOTE: do not clear 'r', because 'r' could be an alias of 'a' or 'b'.  r
  // must already be initialized (and 'a' and 'b', of course)
  DEBUG_CHECK_QUAL_GATE(a);
  DEBUG_CHECK_QUAL_GATE(b);
  DEBUG_CHECK_QUAL_GATE(r);

  gate_word rm[GATE_WORDS];
  memset(rm, 0, sizeof(rm));
  if (!qual_gate_and (a->mask, b->mask, rm)) {
    return FALSE;
  }

  memcpy (r->mask, rm, sizeof (r->mask));
  qual_gate_set_forward(r, forward);
  qual_gate_set_backward(r, backward);

  DEBUG_CHECK_QUAL_GATE(r);
  return TRUE;
}

/* Return TRUE if a passes everything b does */
static inline
bool qual_gate_covers(qual_gate *a, qual_gate *b)
{
  int i;
  int b_empty = TRUE;

  for (i = 0; i < GATE_WORDS_FULL; i++)
    {
      if (b->mask[i] & ~a->mask[i])
	return FALSE;
      if (b->mask[i])
	b_empty = FALSE;
    }

#if GATE_WORDS_PACK_FLAGS
  assert (i == GATE_FLAGWORD_INDEX);
  i = GATE_FLAGWORD_INDEX;

  if (b->mask[i] & ~a->mask[i] & GATE_FLAGWORD_NOFLAG_MASK)
    return FALSE;
  if (b->mask[i] & GATE_FLAGWORD_NOFLAG_MASK)
    b_empty = FALSE;
#endif

  bool a_forward  = qual_gate_get_forward(a);
  bool a_backward = qual_gate_get_backward(a);
  bool b_forward  = qual_gate_get_forward(b);
  bool b_backward = qual_gate_get_backward(b);

  return b_empty || ((a_forward || !b_forward) && (a_backward || !b_backward));
}

// TODO: return a static const char * so we don't have to strcpy.
static void qual_gate_to_string (qual_gate* qgate, char* buf, int len)
{
  static struct { qual_gate *qgate; char* name; } gates[] =
    {
      {&open_qgate, "ALL" },
      {&fi_qgate, "FI" },
      {&fs_qgate, "FS" },
      {&effect_qgate, "EFF" },
      {&casts_preserve_qgate, "CASTS-PR" },
      {&ptrflow_down_pos_qgate, "PTR-D-P" },
      {&ptrflow_down_neg_qgate, "PTR-D-N" },
      {&ptrflow_up_pos_qgate, "PTR-U-P" },
      {&ptrflow_up_neg_qgate, "PTR-U-N" },
      {&fieldflow_down_pos_qgate, "FIELD-D-P" },
      {&fieldflow_down_neg_qgate, "FIELD-D-N" },
      {&fieldflow_up_pos_qgate, "FIELD-U-P" },
      {&fieldflow_up_neg_qgate, "FIELD-U-N" },
      {&fieldptrflow_down_pos_qgate, "FPTR-D-P" },
      {&fieldptrflow_down_neg_qgate, "FPTR-D-N" },
      {&fieldptrflow_up_pos_qgate, "FPTR-U-P" },
      {&fieldptrflow_up_neg_qgate, "FPTR-U-N" }
    };
  int i;

  for (i = 0; i < sizeof (gates) / sizeof (gates[0]); i++)
    if (!memcmp (qgate, gates[i].qgate, sizeof (*qgate)))
      {
	strncpy (buf, gates[i].name, len - 1);
	buf[len-1] = '\0';
	return;
      }

  /* It's not one of the regular ones, so make a custom label */
  strncpy (buf, "CUSTOM", len - 1);
  buf[len-1] = '\0';
}

qual qual_edge_other_end (qual_edge qe, qual q)
{
  DEBUG_CHECK_SUMMARY_LENGTH(qe);
  if (eq_qual (qe->q1, q))
    return qe->q2;
  else
    return qe->q1;
}

// location qual_edge_loc (qual_edge qe)
// {
//   CHECK_SUMMARY_LENGTH(qe);
//   return qe->loc;
// }

static __inline unsigned long hash_qual_edge(qual_edge e)
{
  DEBUG_CHECK_SUMMARY_LENGTH(e);
  unsigned long h = (e->kind + (e->kind != ek_flow ? e->loc_index : 0) +
                     lcprng((unsigned long)ecr_qual(e->q1)) +
                     lcprng((unsigned long)ecr_qual(e->q2)));
  // unsigned long h = e->kind + (e->kind != ek_flow ? e->index : 0) +
  //   (hash_qual(ecr_qual(e->q1)) + (hash_qual(ecr_qual(e->q2))));

  // if (e->DEBUG_prevHash == DEBUG_NO_PREVHASH) {
  //   assert (h != DEBUG_NO_PREVHASH);
  //   printf("## hash_qual_edge(%p): first-time hash = %lu.  q1=%p / %p, q2=%p / %p. kind=%d, index=%d\n",
  //          e, h,
  //          e->q1, ecr_qual(e->q1), e->q2, ecr_qual(e->q2), e->kind, e->index);
  // } else if (e->DEBUG_prevHash != h) {
  //   printf("## hash_qual_edge(%p): hash changed: %lu -> %lu.  q1=%p / %p, q2=%p / %p. kind=%d, index=%d\n",
  //          e, e->DEBUG_prevHash, h,
  //          e->q1, ecr_qual(e->q1), e->q2, ecr_qual(e->q2), e->kind, e->index);
  // }
  // e->DEBUG_prevHash = h;

  return h;
}

int cmp_qual_edge (qual_edge e1, qual_edge e2)
{
  int r;

  DEBUG_CHECK_SUMMARY_LENGTH(e1);
  DEBUG_CHECK_SUMMARY_LENGTH(e2);

  if ((r = cmp_qual (e1->q1, e2->q1)))
    return r;
  if ((r = cmp_qual (e1->q2, e2->q2)))
    return r;
  if ((r = location_cmp(e1->loc, e2->loc)))
    return r;
  // Not sure whether this loc_index comparison is necessary since
  // location_cmp() never considered it.
  // if ((r = e1->loc_index - e2->loc_index))
  //   return r;
  return e1->kind - e2->kind;
}

int cmp_qual_edge_exact (qual_edge e1, qual_edge e2)
{
  DEBUG_CHECK_SUMMARY_LENGTH(e1);
  DEBUG_CHECK_SUMMARY_LENGTH(e2);
  // DEBUG_CHECK_QUAL_EDGE(e1);
  // DEBUG_CHECK_QUAL_EDGE(e2);
  return memcmp (e1, e2, sizeof (*e1));
}

bool eq_qual_edge_exact (qual_edge e1, qual_edge e2)
{
  DEBUG_CHECK_SUMMARY_LENGTH(e1);
  DEBUG_CHECK_SUMMARY_LENGTH(e2);
  // DEBUG_CHECK_QUAL_EDGE(e1);
  // DEBUG_CHECK_QUAL_EDGE(e2);
  return cmp_qual_edge_exact(e1, e2) == 0;
}

static __inline int cmp_qual_edge_semantic(qual_edge e1, qual_edge e2)
{
  int r;
  DEBUG_CHECK_SUMMARY_LENGTH(e1);
  DEBUG_CHECK_SUMMARY_LENGTH(e2);
  if ((r = e1->kind - e2->kind))
    return r;
  if ((r = ecr_qual(e1->q1) - ecr_qual(e2->q1)))
    return r;
  if ((r = ecr_qual(e1->q2) - ecr_qual(e2->q2)))
    return r;
  if (e1->kind != ek_flow && (r = e1->loc_index - e2->loc_index))
    return r;
  if ((r = qual_gate_get_backward(&(e1->qgate)) - qual_gate_get_backward(&(e2->qgate))))
    return r;
  if ((r = qual_gate_get_forward(&(e1->qgate)) - qual_gate_get_forward(&(e2->qgate))))
    return r;
  // quarl 2006-05-28
  //    Only allow summary_time shortcut comparison if it actually is a
  //    summary edge.  Otherwise we can get into a situation where an edge's
  //    ub and lb match up due to summary_time==0, but gates mismatch, so a
  //    third qual_edge compares equal against one but not the other set.
  if (e1->summary_time != 0 && e1->summary_time == e2->summary_time)
    return 0;
  return memcmp(e1->qgate.mask, e2->qgate.mask, sizeof(e1->qgate.mask));
}

bool eq_qual_edge_semantic (qual_edge e1, qual_edge e2)
{
  DEBUG_CHECK_SUMMARY_LENGTH(e1);
  DEBUG_CHECK_SUMMARY_LENGTH(e2);
  return cmp_qual_edge_semantic(e1, e2) == 0;
}

/*
  Make a new edge with symbol s between q1 and q2. All edges are kept
  across queries
 */
static inline bool mkqual_edge(location loc, int loc_index,
                               qedge_kind s, int stime,
			       qual q1, qual q2,
			       qual_gate* qgate, const char *error_message,
			       qual_edge *ne,
                               int edgeNumber)
{
  // DEBUG_CHECK_ALL_EDGE_SETS();
  struct Qual_edge tqe;
  bool retval = FALSE;
  qual_edge result = NULL;

  DEBUG_CHECK_ALL_EDGE_SETS();

  assert (!q1->dead);
  assert (!q2->dead);

  q1 = ecr_qual (q1);
  q2 = ecr_qual (q2);

  assert (!q1->dead);
  assert (!q2->dead);

#if !USE_OWNING_QUAL_EDGE_HASH_TABLES
  assert(((hash_table)q1->u.var.lb)->r == qual_edge_hashsets_region);
  assert(((hash_table)q1->u.var.ub)->r == qual_edge_hashsets_region);
  assert(((hash_table)q2->u.var.lb)->r == qual_edge_hashsets_region);
  assert(((hash_table)q2->u.var.ub)->r == qual_edge_hashsets_region);
#endif

  if (! (qual_gate_get_backward(qgate) || qual_gate_get_forward(qgate)))
    return FALSE;
  if (qual_gate_is_closed (qgate))
    return FALSE;

  if (eq_qual(q1, q2) && (s == ek_flow || global_qual(q1)))
    return FALSE;

  // Must use zero-fill structs if we are to use memcmp to compare two objects
  // -- otherwise the unused bits/bytes (padding) won't match.
  memset(&tqe, 0, sizeof(tqe));
#if DEBUG_EDGE_NUMBER
  tqe.edgeNumber = edgeNumber;
#endif
  tqe.kind = s;
  tqe.summary_time = stime;
  tqe.q1 = q1;
  tqe.q2 = q2;
  tqe.loc = loc;
  assert(loc_index <= MAX_LOC_INDEX);
  tqe.loc_index = loc == NO_LOCATION ? 0 : loc_index;
  tqe.u.error_message = error_message;
  tqe.qgate = *qgate;
  // DEBUG_INIT_QUAL_EDGE(&tqe);
  // DEBUG_CHECK_QUAL_EDGE0(&tqe);
  // printf("## temporary: "); hash_qual_edge(&tqe); /*debug*/

  DEBUG_CHECK_HASH_TABLE(q1->u.var.ub);
  DEBUG_CHECK_HASHSET(q1->u.var.ub);

  if (! qual_edge_hashset_hash_search(q1->u.var.ub, eq_qual_edge_semantic,
        			      &tqe, &result))
    {
      // assert(!hash_table_hash_search_hard((hash_table) q1->u.var.ub, (keyeq_fn) eq_qual_edge_semantic,
      //                                     &tqe, NULL));

      bool inserted;
      // assert(! qual_edge_hashset_hash_search(q2->u.var.lb, eq_qual_edge_semantic,
      //   				     &tqe, &result) &&
      //        "a21cdd0a-cb7c-477f-9501-7eac37fe9407");
      // assert(!hash_table_hash_search_hard((hash_table) q2->u.var.lb, (keyeq_fn) eq_qual_edge_semantic,
      //                                     &tqe, NULL));

#if USE_QUAL_EDGE_MEMPOOL
      result = mempool_alloc(qual_edges_mempool, sizeof(struct Qual_edge));
#else
      result = ralloc(qual_edges_region,struct Qual_edge);
#endif
      memcpy (result, &tqe, sizeof (*result));

      inserted = qual_edge_hashset_insert(&q1->u.var.ub,result);
      assert(inserted);
      inserted = qual_edge_hashset_insert(&q2->u.var.lb,result);
      assert(inserted);

      retval = TRUE;
      qual_stats.qual_edges_created++;
    }
  else
    {
      // quarl: is this right?  what's the point of qual_gate_or() if they're
      // already the same; should the above be checking if everything except
      // gate mask?
      assert((result->summary_time == 0 && stime == 0) ||
             memcmp(result->qgate.mask, qgate->mask,
        	    sizeof(qgate->mask)) == 0);
      qual_gate_or (result->qgate.mask, qgate->mask,
		    result->qgate.mask);
    }

  DEBUG_CHECK_SUMMARY_LENGTH(result);

  if (ne)
    *ne = result;

  DEBUG_CHECK_ALL_EDGE_SETS();

  return retval;
}

static int edge_length(qual_edge qe)
{
  DEBUG_CHECK_SUMMARY_LENGTH(qe);
  if (qe->summary_time)
    return qe->u.summary_length;
  else
    return 1;
}

#if 0
static int total_edge_list_length (qual_edge_list qel)
{
  int tell;
  qual_edge_list_scanner qels;
  qual_edge e;

  if (qel == (qual_edge_list)TRUE)
    return 0;

  tell = 0;
  qual_edge_list_scan (qel, &qels);
  while (qual_edge_list_next(&qels,&e))
    {
      if (e->summary)
	tell += total_edge_list_length (e->summary);
      else
	tell++;
    }

  return tell;
}
#endif

#if 0
static bool find_other_end (qual_edge_list qel, qual this_end, qual* other_end)
{
  if (eq_qual (this_end, (qual_edge_list_head(qel))->q1))
    {
      *other_end = (qual_edge_list_get_tail(qel))->q2;
      return TRUE;
    }
  else
    {
      *other_end = (qual_edge_list_head(qel))->q1;
      return FALSE;
    }
}
#endif

static polarity qedge_kind_to_polarity (qedge_kind ek)
{
  switch (ek)
    {
    case ek_close:
      return p_pos;
    case ek_open:
      return p_neg;
    case ek_flow:
      return p_non;
    default:
      printf ("Unknown edge_kind: %d\n", (int)ek);
    }

  return p_non;
}

/**************************************************************************
 *                                                                        *
 * Conditional Constraint Sets                                            *
 *                                                                        *
 **************************************************************************/

#if USE_CONDS
/* Add condition to conds.  May add duplicates. */
static cond mkcond(cond_dir dir, qual_gate *qgate, qual c, qual left,
		   qual right, location loc, int loc_index,
                   const char * error_message)
{
  cond cond;

  cond = ralloc(quals_permanent_region, struct Condition);
  memset(cond, 0, sizeof(*cond));
  cond->dir = dir;
  cond->c = c;
  cond->qgate = *qgate;
  cond->left = left;
  cond->right = right;
  cond->loc = loc;
  assert(loc_index <= MAX_LOC_INDEX);
  cond->loc_index = loc_index;
  cond->triggered = FALSE;
  cond->error_message = error_message;
  return cond;
}

/* Adds the conditional constraint l1<=r1 ==> l2<=r2.  This constraint
   is allowed only if at least one of {l1,r1} is a constant. */
void cond_mkleq_qual(location loc, int loc_index,
                     qual_gate *qgate1, qual l1, qual r1,
		     qual_gate *qgate2, qual l2, qual r2,
		     const char *error_message)
{
  assert(constant_qual(l1) || constant_qual(r1));
#if DEBUG
  print_qual_raw(printf, l1, &open_qgate);
  printf(" %p <= ", l1);
  print_qual_raw(printf, r1, &open_qgate);
  printf(" %p  ==>  ", r1);
  print_qual_raw(printf, l2, &open_qgate);
  printf(" %p <= ", l2);
  print_qual_raw(printf, r2, &open_qgate);
  printf(" %p\n", r2);
#endif

  l1 = ecr_qual(l1);
  r1 = ecr_qual(r1);
  l2 = ecr_qual(l2);
  r2 = ecr_qual(r2);
  if (leq_qual(l1, r1))
    /* Condition is true */
    mkleq_qual(loc, loc_index, qgate2, l2, r2, error_message);
  else if (! constant_qual(l1) || ! constant_qual(r1))
    {
      /* Condition is not true yet; add constraint to pending set */
      qual v, c;
      cond_dir dir;

      if (constant_qual(l1))
	{ c = l1; v = ecr_qual(r1); dir = c_leq_v; }
      else
	{ c = r1; v = ecr_qual(l1); dir = v_leq_c; }
      assert (constant_qual(c) && variable_qual(v));
      cond_set_insert(quals_permanent_region, &v->u.var.cond_set,
		      mkcond(dir, qgate2, c, l2, r2,
                             loc, loc_index, error_message));
    }
}
#endif

/* If there are any conditional constraint depending on q, trigger
   any newly satisfied ones */
#if 0
static void cond_set_trigger(qual q)
{
  cond_set_scanner css;
  cond cond;

  q = ecr_qual(q);
  assert(q->kind == qk_variable);
  scan_cond_set(cond, css, q->u.var.cond_set)
    if (!cond->triggered)
      {
	switch (cond->dir)
	  {
	  case c_leq_v:
	    cond->triggered = leq_qual(cond->c, q);
	    break;
	  case v_leq_c:
	    cond->triggered = leq_qual(q, cond->c);
	    break;
	  default:
	    fail("Unexpected condition dir %x\n", cond->dir);
	  }
	if (cond->triggered)
	  mkleq_qual(cond->loc, cond->loc_index, cond->kind, cond->left,
		     cond->right, cond->error_message);
      }

}
#endif

/**************************************************************************
 *                                                                        *
 * Error path traversal                                                   *
 *                                                                        *
 **************************************************************************/

/* Find the flow part of a summary path.  Note: this assumes summary
   edges do not cross global nodes. */
static bool find_flow_path (region r, qual start, qual target, qual_gate *qgate,
			    int summary_time, qual_hashset visited,
			    qual_edge_list path)
{
  qual_edge_list worklist;
  qual_edge_list new_worklist;
  qual_edge_pred_map qepm;
  region scratch;
  qual_edge_hashset_scanner qess;
  qual_edge_list_scanner qels;
  qual_edge qe = NULL;
  qual_edge pred = NULL;
  qual_edge last = NULL;
  bool result = FALSE;

  // DEBUG_CHECK_QUAL_EDGE_HASHSET(start->u.var.ub);

  start = ecr_qual(start);
  target = ecr_qual(target);

  if (eq_qual(start, target))
    return TRUE;

  scratch = newregion();
  qepm = make_qual_edge_pred_map(scratch, 16);
  new_worklist = new_qual_edge_list(scratch);

  /* Prime the worklist */
  qual_hashset_insert(&visited, start);
  scan_qual_edge_hashset(qe, qess, start->u.var.ub)
    {
      DEBUG_CHECK_SUMMARY_LENGTH(qe);
      // DEBUG_CHECK_QUAL_EDGE(qe);
      if (qe->kind == ek_flow && qual_gate_covers(&qe->qgate, qgate) &&
	  qe->summary_time < summary_time)
	{
	  new_worklist = qual_edge_list_append_elt(scratch, new_worklist, qe);
	}
    }

  // DEBUG_CHECK_QUAL_EDGE(qe);


  /* Do a bfs. looking for target, and using qepm to keep track of
     all the paths we've found. */
  while (qual_edge_list_length(new_worklist) > 0)
    {
      worklist = new_worklist;
      new_worklist = new_qual_edge_list(scratch);

      qual_edge_list_scan(worklist, &qels);
      while (qual_edge_list_next(&qels, &pred))
	{
	  DEBUG_CHECK_SUMMARY_LENGTH(pred);

	  if (! qual_hashset_member(visited, pred->q2))
	    {
	      qual q = ecr_qual(pred->q2);

	      if (eq_qual(q, target))
		{
		  last = pred;
		  goto DONE;
		}

	      qual_hashset_insert(&visited, q);

	      if (!global_qual(q))
		{
		  scan_qual_edge_hashset(qe, qess, q->u.var.ub)
		    {
		      DEBUG_CHECK_SUMMARY_LENGTH(qe);
		      if (qe->kind == ek_flow && qual_gate_covers(&qe->qgate, qgate) &&
			  ! qual_hashset_member(visited, qe->q2) &&
			  qe->summary_time < summary_time)
			{
			  qual_edge_pred_map_insert(qepm, qe, pred);
			  new_worklist = qual_edge_list_append_elt(scratch, new_worklist, qe);
			}
		    }
		}
	    }
	}
    }

 DONE:

  /* If we found target, reconstruct the path */
  if (last)
    {
      path = qual_edge_list_cons(r, path, last);
      while (qual_edge_pred_map_lookup(qepm, last, &last))
	path = qual_edge_list_cons(r, path, last);
      result = TRUE;
    }

  deleteregion(scratch);
  return result;
}

/* Given a summary edge, rediscover the path it summarizes.  If any of
   the edges in the summarized path are also summary edges, they will
   *not* be replaced by the path they summarize. */
static qual_edge_list find_summary_path (region r, qual_edge e)
{
  qual_edge_list qel = NULL;
  qual_edge_list tmp;
  qual_hashset visited;
  region scratch;
  qual q1;
  qual q2;
  qual_edge_hashset_scanner qess1;
  qual_edge_hashset_scanner qess2;
  qual_edge qe1 = NULL;
  qual_edge qe2 = NULL;

  // DEBUG_CHECK_QUAL_EDGE_HASHSET(e->q1->u.var.ub);
  // DEBUG_CHECK_QUAL_EDGE_HASHSET(e->q2->u.var.ub);

  assert (e->summary_time);
  DEBUG_CHECK_SUMMARY_LENGTH(e);
  q1 = ecr_qual (e->q1);
  q2 = ecr_qual (e->q2);

  // DEBUG_CHECK_QUAL(q1);
  // DEBUG_CHECK_QUAL(q2);

  // DEBUG_CHECK_QUAL_EDGE_HASHSET(q1->u.var.ub);
  // DEBUG_CHECK_QUAL_EDGE_HASHSET(q2->u.var.ub);

  scratch = newregion();
  tmp = new_qual_edge_list (scratch);

  visited = empty_qual_hashset(NULL);

  if (global_qual(q1))
    if (global_qual(q2))
      {
	// visited = empty_qual_hashset(scratch);
        qual_hashset_reset(visited);
	if (find_flow_path(scratch, q1, q2, &e->qgate, e->summary_time, visited, tmp))
	  {
	    qel = qual_edge_list_copy(r, tmp);
	    goto DONE;
	  }
      }

  if (global_qual(q1))
    scan_qual_edge_hashset(qe2,qess2,q2->u.var.lb)
    {
      DEBUG_CHECK_SUMMARY_LENGTH(qe2);
      if (qe2->kind == ek_close && qual_gate_covers(&qe2->qgate, &e->qgate))
	{
          qual_hashset_reset(visited);
	  // visited = empty_qual_hashset(scratch);
	  if (find_flow_path(scratch, q1, ecr_qual(qe2->q1), &e->qgate,
			     e->summary_time, visited, tmp))
	    {
	      qel = qual_edge_list_copy(r, tmp);
	      qel = qual_edge_list_append_elt (r, qel, qe2);
	      goto DONE;
	    }
	}
    }

  if (global_qual(q2))
    scan_qual_edge_hashset(qe1,qess1,q1->u.var.ub)
    {
      DEBUG_CHECK_SUMMARY_LENGTH(qe1);
      if (qe1->kind == ek_open && qual_gate_covers(&qe1->qgate, &e->qgate))
	{
          qual_hashset_reset(visited);
	  // visited = empty_qual_hashset(scratch);
	  if (find_flow_path(scratch, ecr_qual(qe1->q2), q2, &e->qgate,
			     e->summary_time, visited, tmp))
	    {
	      qel = qual_edge_list_copy(r, tmp);
	      qel = qual_edge_list_cons (r, qel, qe1);
	      goto DONE;
	    }
	}
    }

  scan_qual_edge_hashset(qe1,qess1,q1->u.var.ub)
    {
      DEBUG_CHECK_SUMMARY_LENGTH(qe1);

      if (qe1->kind == ek_open && qual_gate_covers(&qe1->qgate, &e->qgate))
	scan_qual_edge_hashset(qe2,qess2,q2->u.var.lb)
	{
	  DEBUG_CHECK_SUMMARY_LENGTH(qe2);

	  if (qe2->kind == ek_close && qe1->loc_index == qe2->loc_index &&
	      qual_gate_covers(&qe2->qgate, &e->qgate))
	    {
              qual_hashset_reset(visited);
	      // visited = empty_qual_hashset(scratch);
	      if (find_flow_path(scratch, ecr_qual(qe1->q2), ecr_qual(qe2->q1),
				 &e->qgate, e->summary_time, visited, tmp))
		{
		  qel = qual_edge_list_copy(r, tmp);
		  qel = qual_edge_list_cons (r, qel, qe1);
		  qel = qual_edge_list_append_elt (r, qel, qe2);
		  goto DONE;
		}
	    }
	}
    }

 DONE:
  deleteregion(scratch);
  destroy_qual_hashset(visited);
  return qel;
}

bool traversing_head;
bool bidi;
edge_traverse_fn current_f;
void* current_arg;

/* munge arguments and call f:
   - q1 is the qualifier we are traversing towards or away from
   - q2 is the next qualifier along the path
   - upper indicates whether q1 <= q2 or vice-versa
   - d indicates whether we are heading towards (d_fwd) q1, or away from it (d_bak)
*/
static void call_f(qual q1, qual q2, location loc, int loc_index,
                   bool upper, polarity p, q_direction d)
{
  qual left;
  qual right;

  q1 = ecr_qual(q1);
  q2 = ecr_qual(q2);

  switch (d)
    {
    case d_fwd:
      left = q1;
      right = q2;
      break;
    case d_bak:
      left = q2;
      right = q1;
      break;
    default:
      fprintf (stderr, "unknown path direction %d.\n", d);
      abort ();
      break;
    }

  if (upper != (d == d_fwd))
    p = -p;

  current_f(left, right, bidi, loc, loc_index, p, traversing_head, current_arg);
  traversing_head = FALSE;
}

static void summary_edge_app(qual_edge qe, bool upper, q_direction d);

struct traverse_error_path_edges_fn_arg
{
  bool upper;
  q_direction d;
};

static void traverse_error_path_edges_fn(qual_edge e, void *arg)
{
  struct traverse_error_path_edges_fn_arg *tepefa = (struct traverse_error_path_edges_fn_arg *)arg;
  qual q1 = ecr_qual(e->q1);
  qual q2 = ecr_qual(e->q2);
  assert(current_f);

  DEBUG_CHECK_SUMMARY_LENGTH(e);
  if (e->summary_time)
    summary_edge_app (e, tepefa->upper, tepefa->d);
  else if (tepefa->upper)
    call_f(q1, q2, e->loc, e->loc_index, tepefa->upper,
	   qedge_kind_to_polarity(e->kind), tepefa->d);
  else
    call_f(q2, q1, e->loc, e->loc_index, tepefa->upper,
	   qedge_kind_to_polarity(e->kind), tepefa->d);
}

static void summary_edge_app(qual_edge qe, bool upper, q_direction d)
{
  region scratch;
  qual_edge_list qel;
  struct traverse_error_path_edges_fn_arg tepefa = { upper, d };

  DEBUG_CHECK_SUMMARY_LENGTH(qe);

  if (eq_qual(qe->q1, qe->q2))
    return;

  DEBUG_CHECK_ALL_EDGE_SETS();

  scratch = newregion();
  qel = find_summary_path(scratch, qe);
  assert (qel);

  if (upper == (d == d_fwd))
    qual_edge_list_app(qel, traverse_error_path_edges_fn, &tepefa);
  else
    qual_edge_list_rev_app(qel, traverse_error_path_edges_fn, &tepefa);

  deleteregion(scratch);
}

/* Apply current_f to every edge on a path from q to b.

   - expand_summaries indicates whether current_f should be applied to
     each edge summarized by a polymorphic summary edge, or whether
     current_f should just be applied to the summary edge itself.
   - same_level == TRUE means don't follow ek_open or ek_close paths
     (which can occur in PN paths)
   - upper indicates whether b is an upper or lower bound of q.
   - if d == d_fwd, the path is traversed starting at q, otherwise starting at b.

   Notes:
   - consecutive calls current_f(left1, right1,...) and current_f(left2, right2,...)
     are guaranteed to have right1 == left2.
*/
static void _traverse_reason_path_edges(qual q, qual b,
					bool expand_summaries, bool same_level,
					bool upper, q_direction d)
{
  qual_bounds qbounds;
  qual_reason qr;
  bool new_upper;

  q = ecr_qual(q);

  qbounds = q->u.var.qbounds;

  /* Look up the first step along the path. */
  if (upper)
    qual_reason_map_lookup(qbounds->ubr, b, &qr);
  else
    qual_reason_map_lookup(qbounds->lbr, b, &qr);

  /* The base case: we've reached b */
  if (qr->kind == rk_location)
    {
      call_f(q, b, qr->u.s.loc, qr->u.s.loc_index, upper, p_non, d);
      return;
    }

  if (qr->u.e.qe->kind != ek_flow && same_level)
    return;

  DEBUG_CHECK_SUMMARY_LENGTH(qr->u.e.qe);
  /* Determine if this edge leads to or from q, and remember this fact
     in new_upper. */
  new_upper = eq_qual(q, qr->u.e.qe->q1);

  /* If we want to head away from q */
  if (d == d_fwd)
    {
      if (qr->u.e.qe->summary_time && expand_summaries)
	summary_edge_app (qr->u.e.qe, new_upper, d);
      else
	call_f(q, qual_edge_other_end(qr->u.e.qe,q),
               qr->u.e.qe->loc, qr->u.e.qe->loc_index,
	       new_upper, qedge_kind_to_polarity(qr->u.e.qe->kind), d);
    }

  _traverse_reason_path_edges(qual_edge_other_end(qr->u.e.qe, q), b,
			      expand_summaries, same_level, new_upper, d);

  /* If we want to head towards q */
  if (d == d_bak)
    {
      if (qr->u.e.qe->summary_time && expand_summaries)
	summary_edge_app (qr->u.e.qe, new_upper, d);
      else
	call_f(q, qual_edge_other_end(qr->u.e.qe, q),
               qr->u.e.qe->loc, qr->u.e.qe->loc_index,
	       new_upper, qedge_kind_to_polarity(qr->u.e.qe->kind), d);
    }
}

static void _setup_traverse_reason_path_edges(qual bound,
					      edge_traverse_fn f, void *arg)
{
  current_f = f;
  current_arg = arg;
  traversing_head = TRUE;
  bidi = bound->u.elt.sign == sign_eq;
}

static void _traverse_error_path_edges(qual q, bool context_summary,
				       bool expand_summaries, bool nonerrors,
				       edge_traverse_fn f, void *arg)
{
  qual bound;
  qual_reason qr;
  qual_reason_map_scanner qrms;
  qual_error_list qel;
  qual_error_list redundant;
  qual_set printed_bounds;
  qual_error_list_scanner scan;
  qual_error next_error = NULL;
  region scratch;

  q = ecr_qual(q);
  assert(q->kind == qk_variable);

  scratch = newregion();

  qel = new_qual_error_list (scratch);
  redundant = new_qual_error_list (scratch);
  printed_bounds = empty_qual_set(scratch);
  _may_leq_qual (q, q, context_summary, &qel, scratch);

  qual_error_list_scan(qel,&scan);

  /* First, do the errors */
  while(qual_error_list_next(&scan,&next_error))
    {
      bool dup = FALSE;
      qual loffender = next_error->lbc;
      qual uoffender = next_error->ubc;

      /* Prevent errors involving sign_eq qualifiers from being
	 explored twice (once in each direction) */
      if (sign_qual(loffender) == sign_eq && sign_qual(uoffender) == sign_eq)
	{
	  qual_error_list_scanner rscan;
	  qual_error rerror = NULL;

	  qual_error_list_scan(redundant,&rscan);
	  while(qual_error_list_next(&rscan,&rerror))
	    if (eq_qual(rerror->lbc, uoffender) && eq_qual(rerror->ubc, loffender))
	      {
		dup = TRUE;
		break;
	      }
	}
      if (dup)
	continue;
      redundant = qual_error_list_cons(scratch, redundant, next_error);
      qual_set_insert(scratch, &printed_bounds, uoffender);
      qual_set_insert(scratch, &printed_bounds, loffender);

      if (loffender)
	{
	  _setup_traverse_reason_path_edges(loffender, f, arg);
	  _traverse_reason_path_edges(q,loffender,expand_summaries,FALSE,FALSE, d_bak);
	}

      if (uoffender)
	{
	  _setup_traverse_reason_path_edges(uoffender, f, arg);
	  traversing_head = FALSE;
	  _traverse_reason_path_edges(q,uoffender,expand_summaries,FALSE,TRUE, d_fwd);
	}
    }

  /* Now print the paths for any qual bounds that aren't involved in
     errors */
  if (nonerrors && q->u.var.qbounds)
    {
      scan_qual_reason_map(bound, qr, qrms, q->u.var.qbounds->lbr)
	if (sign_qual(bound) != sign_neg &&
	    !qual_set_member (printed_bounds, bound))
	  {
	    _setup_traverse_reason_path_edges(bound, f, arg);
	    _traverse_reason_path_edges(q,bound,expand_summaries,FALSE,FALSE,d_bak);
	    qual_set_insert(scratch, &printed_bounds, bound);
	  }

      scan_qual_reason_map(bound, qr, qrms, q->u.var.qbounds->ubr)
	if (sign_qual(bound) == sign_neg &&
	    !qual_set_member (printed_bounds, bound))
	  {
	    _setup_traverse_reason_path_edges(bound, f, arg);
	    _traverse_reason_path_edges(q,bound,expand_summaries,FALSE,TRUE,d_bak);
	    qual_set_insert(scratch, &printed_bounds, bound);
	  }
    }

  deleteregion (scratch);
}

void traverse_error_path_edges(qual q, bool expand_summaries, bool nonerrors,
			       edge_traverse_fn f, void *arg)
{
  return _traverse_error_path_edges (q, FALSE, expand_summaries, nonerrors, f, arg);
}

void cs_traverse_error_path_edges(qual q, bool expand_summaries, bool nonerrors,
				  edge_traverse_fn f, void *arg)
{
  return _traverse_error_path_edges (q, TRUE, expand_summaries, nonerrors, f, arg);
}

/**************************************************************************
 *                                                                        *
 * Graph file generation and other debugging output                       *
 *                                                                        *
 **************************************************************************/

static FILE* print_graph_file_target;

#if 0
static const char *qedge_kind_to_string(qedge_kind k)
{
  switch (k)
    {
    case ek_flow:
      return "M";
    case ek_open:
      return "(";
    case ek_close:
      return ")";
    }
  assert(0);
  return "ERROR";
}
#endif

#if 0
static const char *q_direction_to_string(q_direction d)
{
  switch (d)
    {
    case d_fwd:
      return "f";
    case d_bak:
      return "b";
    default:
      fprintf (stderr, "unknown path direction %d.\n", d);
      abort ();
    }
  assert (0);
  return "ERROR";
}
#endif

const char *polarity_to_string(polarity p)
{
  switch (p)
    {
    case p_neg:
      return "-";
    case p_pos:
      return "+";
    case p_non:
      return "T";
    case p_sub:
      return "S";
    default:
      fprintf (stderr, "unknown polarity %d.\n", p);
      abort ();
    }
  assert(0);
  return "ERROR";
}

static int print_graph_file(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  return vfprintf(print_graph_file_target, fmt, args);
}

static void print_graph_name (FILE* f, const qual q)
{
  print_qual_raw(print_graph_file, q, &open_qgate);
/* #ifdef DEBUG */
  /* was  if (flag_ugly) */
    print_graph_file ("(%p)", q);
/* #endif */
  if (q->kind == qk_variable && q->u.var.global)
    print_graph_file ("[g]");
}

static void add_graph_node (FILE* f, const qual q)
{
  print_graph_file_target = f;
  fprintf(f, "\"");
  print_graph_name (f, q);
  fprintf(f, "\"\n");
}

static void add_graph_edge(FILE* f, const qual left, const qual right,
		    const char* label, const bool dotted)
{
  print_graph_file_target = f;

  fprintf(f, "\"");
  print_graph_name(f, left);
  fprintf(f, "\"->\"");
  print_graph_name(f, right);
  fprintf(f, "\"");
  fprintf (f, "[");
  if (label)
    fprintf(f, "label=\"%s\",", label);
  fprintf(f, "style=%s", dotted ? "dotted" : "solid");
  fprintf (f, "]\n");
}

static char* location_label (location loc)
{
  static char buf[1024];
  snprintf (buf, sizeof(buf), "%s:%ld",
	    location_filename(loc), location_line_number(loc));
  buf[sizeof(buf)-1] = '\0';
  return buf;
}

static char* edge_label(const qual_edge e)
{
  static char gate_buf[1024];
  static char buf[1024];

  DEBUG_CHECK_SUMMARY_LENGTH(e);
  qual_gate_to_string (&e->qgate, gate_buf, sizeof (gate_buf));
  snprintf (buf, 1000,
            "%d,%s,%s"
#if DEBUG_EDGE_NUMBER
            " [#%d]"
#endif
	    " %s"
            ,
            e->loc_index,
	    polarity_to_string(qedge_kind_to_polarity(e->kind)),
	    gate_buf,
#if DEBUG_EDGE_NUMBER
            e->edgeNumber,
#endif
	    flag_ugly ? ptr_to_ascii(e) : ""
            );
  buf[sizeof(buf)-1] = '\0';
  return buf;
}

static void dump_quals_graph_constant_bounds (FILE* f, qual q)
{
  if (q->u.var.qbounds)
    {
      qual_reason_map_scanner qrms;
      qual qc;
      qual_reason qr;

      scan_qual_reason_map (qc, qr, qrms, q->u.var.qbounds->lbr)
	{
	  if (qr->ci_bound)
	    {
	      if (qr->kind == rk_location)
		add_graph_edge (f, qc, q, location_label (qr->u.s.loc), FALSE);
	      else
		add_graph_edge (f, qc, q, "*", FALSE);
	    }
	}

      scan_qual_reason_map (qc, qr, qrms, q->u.var.qbounds->ubr)
	{
	  if (qr->ci_bound)
	    {
	      if (qr->kind == rk_location)
		add_graph_edge (f, q, qc, location_label (qr->u.s.loc), FALSE);
	      else
		add_graph_edge (f, q, qc, "*", FALSE);
	    }
	}
    }
}

static void _dn_dfs (FILE* f, region scratch,
	      qual root, int depth, qual_edge_set* printed_edges)
{
  qual_edge_hashset_scanner qess;
  qual_edge e;

  assert (root->kind == qk_variable);

  if (root->u.var.visited)
    return;
  root->u.var.visited = TRUE;

  if (root->link)
    {
      add_graph_edge (f, root, ecr_qual (root), "+", TRUE);
      return _dn_dfs (f, scratch, ecr_qual (root), depth, printed_edges);
    }

  scan_qual_edge_hashset (e, qess, root->u.var.lb)
    {
      qual q = ecr_qual (e->q1);
      if (! q->link)
	{
	  if ((depth > 0 || q->u.var.visited) &&
	      ! qual_edge_set_member (cmp_qual_edge, *printed_edges, e))
	    {
	      add_graph_edge (f, q, root, edge_label (e), FALSE);
	      qual_edge_set_insert (scratch, printed_edges, e);
	    }

	  if (depth > 0)
	    _dn_dfs (f, scratch, q, depth-1, printed_edges);
	}
    }

  scan_qual_edge_hashset (e, qess, root->u.var.ub)
    {
      qual q = ecr_qual (e->q2);
      if (! q->link)
	{
	  if ((depth > 0 || q->u.var.visited) &&
	      ! qual_edge_set_member (cmp_qual_edge, *printed_edges, e))
	    {
	      add_graph_edge (f, root, q, edge_label (e), FALSE);
	      qual_edge_set_insert (scratch, printed_edges, e);
	    }

	  if (depth > 0)
	    _dn_dfs (f, scratch, q, depth-1, printed_edges);
	}
    }

  dump_quals_graph_constant_bounds (f, root);
}

static void _dump_neighborhood (FILE* f, qual_set vars, qual start, int size)
{
  qual_edge_set printed_edges;
  region scratch;

  reset_visited (vars);

  scratch = newregion();

  printed_edges = empty_qual_edge_set(scratch);

  _dn_dfs (f, scratch, start, size, &printed_edges);

  deleteregion (scratch);
}

static void dump_graph_init (FILE* f)
{
  fprintf(f, "digraph G {\n");
}

static void dump_graph_finish (FILE* f)
{
  fprintf (f, "}\n");
}

static void dump_neighborhood (FILE* f, qual_set vars, qual root, int depth)
{
  dump_graph_init (f);
  _dump_neighborhood (f, vars, root, depth);
  dump_graph_finish (f);
}

static void declare_all_qvars_need_names();
static void dump_quals_graph (FILE* f, qual_set vars)
{
  qual_set_scanner qss;
  qual q;
  qual_edge_hashset_scanner qess;
  qual_edge e;

  dump_graph_init (f);

#if DELAY_SET_NAMES
  declare_all_qvars_need_names();
#endif

  scan_qual_set (q, qss, vars)
    {
      if (q->kind == qk_link)
	continue;

      add_graph_node (f, q);

      scan_qual_edge_hashset (e, qess, q->u.var.ub)
	add_graph_edge (f, q, ecr_qual(e->q2), edge_label (e), FALSE);

      dump_quals_graph_constant_bounds (f, q);
    }


  dump_graph_finish (f);

}

#if 0
static void print_route(qual_edge_list el)
{
  qual_edge_list_scanner scan;
  qual_edge temp;

  qual_edge_list_scan(el,&scan);

  if(qual_edge_list_next(&scan,&temp))
    {
      print_qual_raw(printf,temp->q1);
      if (temp->kind == ek_flow)
	  printf(" --%s--> ",qedge_kind_to_string(temp->kind));
	else
	  printf(" --%s%d--> ",qedge_kind_to_string(temp->kind),temp->index);
      print_qual_raw(printf,temp->q2);
    }

  while(qual_edge_list_next(&scan,&temp))
    {
      if (temp->kind == ek_flow)
	printf(" --%s--> ",qedge_kind_to_string(temp->kind));
      else
	printf(" --%s%d--> ",qedge_kind_to_string(temp->kind),temp->index);
      print_qual_raw(printf,temp->q2);
    }
  putchar('\n');
}
#endif

#ifdef DEBUG
static void dump_qual_set (qual_set s)
{
  qual_set_scanner qss;
  qual q;

  printf ("{ ");
  scan_qual_set (q, qss, s)
    {
      if (q->link)
	{
	  printf ("%s[%s]<0x%X>, ", q->u.var.name, ecr_qual(q)->u.var.name,
		  (int)q);
	}
      else
	{
	  printf ("%s<0x%X>, ", q->u.var.name, (int)q);
	}
    }
  printf ("}\n");
}
#endif

/**************************************************************************
 *                                                                        *
 * Constraint generation                                                  *
 *                                                                        *
 **************************************************************************/

#if 0
/* For determining the best reason a constant bounds a variable */
static int total_path_length (qual_edge qe, qual var, qual constant, bool upper)
{
  qual_reason qr;
  bool found_other_end;
  int tpl;

  tpl = 0;

  /* Add the length of this path */


  /* Add the length of the path from the other end to the constant */
  if (eq_qual (var, p->q1))
    var = p->q2;
  else
    var = p->q1;

  if (upper)
    found_other_end = qual_reason_map_lookup (var->u.var.qbounds->ubr, constant, &qr);
  else
    found_other_end = qual_reason_map_lookup (var->u.var.qbounds->lbr, constant, &qr);

  if (found_other_end)
    if (qr->kind == rk_path)
      tpl += qr->u.path.totalpathlength;

  return tpl;
}
#endif

static inline bool insert_X_bound (location loc, int loc_index,
				   qual constant, qual var,
				   qual_edge qe,
				   bool upper, bool ci_bound,
                                   int edgeNumber)
{
  bool result = FALSE;
  qual_bounds bounds;
  qual_reason qr;
  bool already_bounds;
  bool bound_upgrade;

  var = ecr_qual(var);
  assert(constant->kind == qk_constant && var->kind == qk_variable);
  assert(loc!=NO_LOCATION || qe);

  /* Alloc a new bounds structure if this is the first bound on this
     variable */
  if (!(bounds = var->u.var.qbounds))
    {
      bounds = ralloc(quals_permanent_region, struct Qual_bounds);
      memset(bounds, 0, sizeof(*bounds));
      bounds->lbr = make_qual_reason_map (quals_permanent_region, 2);
      bounds->ubr = make_qual_reason_map (quals_permanent_region, 2);
      var->u.var.qbounds = bounds;
      // TODO: analyze whether it's better to use an owning hash_table here.
      // The advantage of owning is that it doesn't leak when expanding or
      // getting rid of it.  But it may not be a big deal if we don't often
      // expand.
    }

  /* If this is the first time this constant is a bound on this
     variable, allocate a reason for this bound */
  if (upper)
    already_bounds = qual_reason_map_lookup(bounds->ubr, constant, &qr);
  else
    already_bounds = qual_reason_map_lookup(bounds->lbr, constant, &qr);

  if (!already_bounds)
    {
      qr = ralloc (quals_permanent_region, struct Qual_reason);
      memset(qr, 0, sizeof(*qr));
      qr->kind = rk_none;
      qr->ci_bound = FALSE;
#if DEBUG_EDGE_NUMBER
      qr->edgeNumber = edgeNumber;
#endif
      if (upper)
	{
	  qual_reason_map_insert (bounds->ubr, constant, qr);
	}
      else
	{
	  qual_reason_map_insert (bounds->lbr, constant, qr);
	}
      result = TRUE;
    }

  bound_upgrade = !qr->ci_bound && ci_bound;
  if (bound_upgrade)
    result = TRUE;
  qr->ci_bound |= ci_bound;

  /* Now store the "best" reason for this constant bound.  locations
     (i.e. original bounds) are better than derived bounds. Derived
     bounds with shorter paths are better than those with longer
     paths.  Any edge path is better than no reason. */
  if (loc!=NO_LOCATION && (qr->kind < rk_location || bound_upgrade))
    {
      qr->kind = rk_location;
      qr->u.s.loc = loc;
      assert(loc_index <= MAX_LOC_INDEX);
      qr->u.s.loc_index = loc_index;
      result = TRUE;
    }
  else if (qe && (qr->kind < rk_edge || bound_upgrade))
    {
      qual q2;
      qual_reason qr2;

      qr->kind = rk_edge;
      qr->u.e.qe = qe;
      DEBUG_CHECK_SUMMARY_LENGTH(qe);

      q2 = ecr_qual(qual_edge_other_end(qe, var));
      assert(variable_qual(q2) && q2->u.var.qbounds);
      if (upper)
	insist(qual_reason_map_lookup(q2->u.var.qbounds->ubr, constant, &qr2));
      else
	insist(qual_reason_map_lookup(q2->u.var.qbounds->lbr, constant, &qr2));

      qr->u.e.pathlength = edge_length(qr->u.e.qe);
      if (qr2->kind == rk_edge)
	qr->u.e.pathlength += qr2->u.e.pathlength;

#ifdef DEBUG
      if (upper)
	printf ("IXB: %s <= %s: ", constant->u.elt.name, var->u.var.name);
      else
	printf ("IXB: %s <= %s: ", var->u.elt.name, constant->u.var.name);
      /*print_path (p);*/
      printf ("\n");
#endif

      result = TRUE;
    }

#if USE_TRIGGERS
  /* If this is a new bound, trigger propagation through stores */
  if (upper && !already_bounds && var->u.var.ub_closure)
    invoke_closure(var->u.var.ub_closure);
  else if (!upper && !already_bounds && var->u.var.lb_closure)
    invoke_closure(var->u.var.lb_closure);
#endif
  return result;
}

/*
  The constant q1 is inserted into q2's lower bounds, and q2 is marked
  interesting
 */
static inline bool insert_lower_bound(location loc, int loc_index,
                                      qual q1, qual q2,
				      qual_edge qe,
				      bool ci_bound,
                                      int edgeNumber)
{
  if (insert_X_bound (loc, loc_index, q1, q2, qe, FALSE, ci_bound, edgeNumber))
    return INSERT_LOWER;
  else
    return INSERT_NOTHING;
}

/*
  The constant q2 is inserted into q1's upper bounds, and q1 is marked
  interesting. We also note that q1 could be involved in an inconsistent
  constraint, since it has an upper bound
 */
static inline bool insert_upper_bound(location loc, int loc_index,
                                      qual q1, qual q2,
				      qual_edge qe,
				      bool ci_bound,
                                      int edgeNumber)
{
  if (insert_X_bound (loc, loc_index, q2, q1, qe, TRUE, ci_bound, edgeNumber))
    return INSERT_UPPER;
  else
    return INSERT_NOTHING;

}

/* called internally. this does not imply any transitive constraints,
   though qinstantiations may induce unifications due to well-formedness */
static void _mkinst_qual_en(location loc, int loc_index,
                            qual_gate *qgate, qual left, qual right,
                            polarity p, const char *error_message, int edgeNumber)
{
  qinstantiation inst;
  qual q1 = ecr_qual(left);
  qual q2 = ecr_qual(right);

  if (!flag_poly)
    {
      if (p != p_non)
	{
	  mkleq_qual_en (loc, loc_index, qgate, left, right, error_message, edgeNumber);
	}
      else
	{
	  mkleq_qual_en (loc, loc_index, qgate, left, right, error_message, edgeNumber);
	  mkleq_qual_en (loc, loc_index, qgate, right, left, error_message, edgeNumber);
	}
    }

#if DEBUG_CONSTRAINT
  print_qual_raw(printf, left, &open_qgate);
  printf(" %p (=%d,%s ", left, loc_index, polarity_to_string(p));
  print_qual_raw(printf, right, &open_qgate);
  printf(" %p\n", right);
#endif

  left = right = NULL; /* Only use q1 and q2 below. */

  assert(q1->kind == qk_variable);
  assert(q2->kind == qk_variable);

  inst = ralloc(resolve_region,struct QInstantiation);
  memset(inst, 0, sizeof(*inst));
  inst->loc = loc;
  assert(loc_index <= MAX_LOC_INDEX);
  inst->loc_index = loc_index;
  inst->p = p;
  inst->q1 = q1;
  inst->q2 = q2;
  qinstantiation_list_cons(resolve_region, qinstantiations, inst);

  if (p == p_pos)
    {
      mkqual_edge(loc, loc_index,
                  ek_close,0,q1,q2, qgate, error_message, NULL, edgeNumber);
      q1->u.var.ret = TRUE;
      q2->u.var.returned = TRUE;

       /* insert_ret(q1,e); */
    }
  else if (p == p_neg)
    {
      mkqual_edge(loc, loc_index,
                  ek_open,0,q1,q2, qgate, error_message, NULL, edgeNumber);
      q1->u.var.passed = TRUE;
      q2->u.var.param = TRUE;

       /* insert_param(q1,e); */
    }
  else /* p_non */
    {
      mkqual_edge(loc, loc_index,
                  ek_flow,0,q1,q2, qgate, error_message, NULL, edgeNumber);
      mkqual_edge(loc, loc_index,
                  ek_flow,0,q2,q1, qgate, error_message, NULL, edgeNumber);
    }

  DEBUG_CHECK_ALL_VARS();
}

/* left (= right with polarity p at the index described by loc.
   return true if an error occurred */
void mkinst_qual(location loc, int loc_index,
                 qual_gate *qgate, qual left, qual right, polarity p,
                 const char *error_message)
{
  // CHECK_ALL_EDGE_SETS();
  mkinst_qual_en(loc, loc_index, qgate, left, right, p, error_message, -1);
  // CHECK_ALL_EDGE_SETS();
}

void mkinst_qual_en(location loc, int loc_index,
                    qual_gate *qgate, qual left, qual right, polarity p,
                    const char *error_message,
                    int edgeNumber /* unique id for the edge */
                    )
{
  // CHECK_ALL_EDGE_SETS();
  _mkinst_qual_en(loc,loc_index,qgate,left,right,p, error_message, edgeNumber);
  // CHECK_ALL_EDGE_SETS();
}

static int _mkleq_qual_en(location loc, int loc_index,
                          qual left, qual right,
                          bool orig, bool ci_bound,
                          qual_gate* qgate, const char* error_message,
                          qual_edge *new_edge, qual_edge causal_edge,
                          int edgeNumber)
{
  // DEBUG_CHECK_ALL_EDGE_SETS();
  bool result = INSERT_NOTHING;
  qual q1 = ecr_qual(left);
  qual q2 = ecr_qual(right);
  // DEBUG_CHECK_ALL_EDGE_SETS();

  assert (!q1->link);
  assert (!q2->link);

#if DEBUG_CONSTRAINT
 {
#define MAX_QGATE 50
   char qual_gate_name[MAX_QGATE];
   qual_gate_to_string(qgate, qual_gate_name, MAX_QGATE);
   print_qual_raw(printf, left, &open_qgate);
   printf(" %p <=(%s) ", left, qual_gate_name);
   print_qual_raw(printf, right, &open_qgate);
   printf(" %p\n", right);
 }
#endif

  if (new_edge)
    *new_edge = NULL;

  /* Two constants, should never happen */
  if (q1->kind == qk_constant && q2->kind == qk_constant)
    {
      fail("mkleq_qual called on two qualifier constants");
    }
  else if (q1->kind == qk_constant && q2->kind == qk_variable)
    {
      result |= insert_lower_bound(loc, loc_index,
                                   q1,q2, causal_edge, ci_bound, edgeNumber);

      if (sign_qual(q1) == sign_eq || qual_set_empty (q1->u.elt.ubc))
	result |= insert_upper_bound(loc, loc_index,
                                     q2,q1, causal_edge, ci_bound, edgeNumber);
  // DEBUG_CHECK_ALL_EDGE_SETS();
    }
  else if (q1->kind == qk_variable && q2->kind == qk_constant)
    {
      result |= insert_upper_bound(loc, loc_index,
                                   q1,q2, causal_edge, ci_bound, edgeNumber);

      if (sign_qual(q2) == sign_eq || qual_set_empty (q2->u.elt.lbc))
	result |= insert_lower_bound(loc, loc_index,
                                     q2,q1, causal_edge, ci_bound, edgeNumber);
  // DEBUG_CHECK_ALL_EDGE_SETS();
    }
  else if (q1->kind == qk_variable && q2->kind == qk_variable)
    {
      mkqual_edge(loc,loc_index,
                  ek_flow,0,q1,q2, qgate, error_message, new_edge, edgeNumber);
  // DEBUG_CHECK_ALL_EDGE_SETS();

       /* CHECK_ALL_VARS(); */
       /* result = qual_edge_set_insert(quals_permanent_region,&q1->u.var.ub,e); */
       /* CHECK_ALL_VARS(); */
       /* result |= qual_edge_set_insert(quals_permanent_region,&q2->u.var.lb,e); */
       /* CHECK_ALL_VARS(); */
      result |= INSERT_EDGE;
    }
  // DEBUG_CHECK_ALL_EDGE_SETS();

  return result;
}

/* called internally, orig is true if the constraint is not derived
   transitively.  makes a new intraprocedural path edge between left
   and right, if left and right are both vars. if one of left or right
   is a constant, it is added to lbc or ubc respectively via
   insert_X_bound */
static int _mkleq_qual(location loc, int loc_index,
                       qual left, qual right,
                       bool orig, bool ci_bound,
                       qual_gate* qgate, const char* error_message,
                       qual_edge *new_edge, qual_edge causal_edge)
{
  return _mkleq_qual_en(loc, loc_index, left, right, orig, ci_bound,
                        qgate, error_message, new_edge, causal_edge,
                        -1);
}

/* left <= right, return true if an error occurred */
void mkleq_qual(location loc, int loc_index,
                qual_gate *qgate, qual left, qual right,
                const char *error_message)
{
  return mkleq_qual_en(loc, loc_index, qgate, left, right, error_message, -1);
}

void mkleq_qual_en(location loc, int loc_index,
                   qual_gate *qgate, qual left, qual right,
                   const char *error_message,
                   int edgeNumber  /* unique id for the edge */
                   )
{
  /* dsw: watch what is happening. */
  _mkleq_qual_en(loc,loc_index,
                 left,right,TRUE,TRUE,qgate,error_message,NULL,NULL,edgeNumber);
  DEBUG_CHECK_ALL_VARS();
  DEBUG_CHECK_ALL_EDGE_SETS();
}

/* CHECK -- need to unify in order to make the instantiations well formed */

/* left == right, return true if an error occurred */
void mkeq_qual(location loc, int loc_index,
               qual_gate *qgate, qual left, qual right,
               const char *error_message)
{
  return mkeq_qual_en(loc, loc_index,
                      qgate, left, right, error_message, -1);
}

void mkeq_qual_en(location loc, int loc_index,
                  qual_gate *qgate, qual left, qual right,
                  const char *error_message,
                  int edgeNumber   /* unique id for the edge */
                  )
{
  /* dsw: watch what is happening. */
#if 1
  mkleq_qual_en(loc, loc_index, qgate, left, right, error_message, edgeNumber);
  mkleq_qual_en(loc, loc_index, qgate, right, left, error_message, edgeNumber);
  DEBUG_CHECK_ALL_EDGE_SETS();
#else
  // DEBUG_CHECK_ALL_VARS();
  unify_qual_en(loc,loc_index,left,right, edgeNumber);
  DEBUG_CHECK_ALL_VARS();
#endif
}

static void bounds_union(qual_bounds new, qual_bounds old)
{
  qual tq;
  qual_reason_map_scanner qrms;
  qual_reason tqr1;
  qual_reason tqr2;

  scan_qual_reason_map (tq, tqr1, qrms, old->lbr)
    if (!qual_reason_map_lookup(new->lbr, tq, &tqr2))
      qual_reason_map_insert (new->lbr, tq, tqr1);
  scan_qual_reason_map (tq, tqr1, qrms, old->ubr)
    if (!qual_reason_map_lookup(new->ubr, tq, &tqr2))
      qual_reason_map_insert (new->ubr, tq, tqr1);
}

/* left == right, plus indistinguishable afterwards,
   return true if an error occurred */
void unify_qual(location loc, int loc_index,
                qual left, qual right, const char * error_message)
{
  unify_qual_en(loc, loc_index, left, right, error_message, -1);
}

/* left == right, plus indistinguishable afterwards,
   return true if an error occurred */
void unify_qual_en(location loc, int loc_index,
                   qual left, qual right, const char * error_message,
                   int edgeNumber)
{
  // static int count = 0;
  // count++;
  // DEBUG_CHECK_QUAL(left);
  // DEBUG_CHECK_QUAL(right);
   /* qual_bounds link_bounds, ecr_bounds; */

  assert (!left->dead);
  assert (!right->dead);

  left = ecr_qual(left);
  right = ecr_qual(right);

  assert (!left->dead);
  assert (!right->dead);

  DEBUG_CHECK_ALL_EDGE_SETS();

  if (left == right) /* Short circuit to prevent loops in ecr_qual */
    return;

  (*pquals_unified)++;

#if DEBUG_CONSTRAINT
  print_qual_raw(printf, left, &open_qgate);
  printf(" %p == ", left);
  print_qual_raw(printf, right, &open_qgate);
  printf(" %p\n", right);
#endif

  mkeq_qual_en(loc, loc_index,
               &open_qgate, left, right, error_message, edgeNumber);
  // CHECK_ALL_EDGE_SETS();

  if (left->kind == qk_variable && right->kind == qk_variable)
    {
      qual new_ecr, new_link;
      qual_edge_hashset_scanner qess;
      qual_edge qe;

      if (right->u.var.preferred || left->u.var.anonymous ||
	  (left->num_equiv <= right->num_equiv))
	{
	  new_ecr = right;
	  new_link = left;
	}
      else
	{
	  new_ecr = left;
	  new_link = right;
	}

#if DELAY_SET_NAMES
      // We already automatically get the preferred name if there is one since
      // it is the ECR; TODO: does the left->num_equiv<=right->num_equiv test
      // guarantee choosing a name independent of order of unifications?
#else
      /* Always try to pick a preferred name.  As a fall-back, choose
	 the name that comes first alphabetically.  That way the name
	 of the ecr is independent of the order of the
	 unifications. */
      if (preferred_qual(left) && !preferred_qual(right))
	new_ecr->u.var.name = left->u.var.name;
      else if (!preferred_qual(left) && preferred_qual(right))
	new_ecr->u.var.name = right->u.var.name;
      else if (right->u.var.name && !left->u.var.name)
	new_ecr->u.var.name = right->u.var.name;
      else if (left->u.var.name && !right->u.var.name)
	new_ecr->u.var.name = left->u.var.name;
      else if (left->u.var.name && right->u.var.name && strcmp (left->u.var.name, right->u.var.name) > 0)
	new_ecr->u.var.name = right->u.var.name;
      else
      {
	// new_ecr->u.var.name = new_ecr->u.var.name;
      }
#endif

      if (new_link->u.var.qbounds)
	{
	  if (new_ecr->u.var.qbounds)
	    {
	      bounds_union(new_ecr->u.var.qbounds,new_link->u.var.qbounds);
	    }
	  else
	    {
	      new_ecr->u.var.qbounds = new_link->u.var.qbounds;
	    }
	}

      assert(new_ecr->num_equiv + new_link->num_equiv <= MAX_NUM_EQUIV);
      new_ecr->num_equiv += new_link->num_equiv;
      new_ecr->u.var.preferred |= new_link->u.var.preferred;
      new_ecr->u.var.param |= new_link->u.var.param;
      new_ecr->u.var.passed |= new_link->u.var.passed;
      new_ecr->u.var.returned |= new_link->u.var.returned;
      new_ecr->u.var.ret |= new_link->u.var.ret;
      new_ecr->u.var.bak_seeded |= new_link->u.var.bak_seeded;
      new_ecr->u.var.fwd_seeded |= new_link->u.var.fwd_seeded;
      new_ecr->u.var.global |= new_link->u.var.global;
      new_ecr->u.var.isextern |= new_link->u.var.isextern;
      new_ecr->u.var.defined |= new_link->u.var.defined;
      new_ecr->u.var.anonymous &= new_link->u.var.anonymous;
      new_link->link = new_ecr;
      new_link->kind = qk_link;
      /* It's better to do these after the link is set up. */

      new_ecr->u.var.lb =
        qual_edge_hashset_union(new_ecr->u.var.lb, new_link->u.var.lb);
      new_ecr->u.var.ub =
        qual_edge_hashset_union(new_ecr->u.var.ub, new_link->u.var.ub);

#if USE_QUAL_EDGE_MEMPOOL
      // TODO: deallocate the duplicate edges
#endif

      // quarl: Make sure these are no longer used, and also reclaim memory.
#if USE_OWNING_QUAL_EDGE_HASH_TABLES && RELEASE_QUAL_EDGE_HASH_TABLE_MEMORY
      destroy_qual_edge_hashset(new_link->u.var.lb);
      destroy_qual_edge_hashset(new_link->u.var.ub);
#endif
      new_link->u.var.lb = NULL;
      new_link->u.var.ub = NULL;

      scan_qual_edge_hashset(qe, qess, new_ecr->u.var.lb) {
	if (!eq_qual(new_ecr, qe->q1)) {
          // printf("## unify_qual_en: updating hashes for %p\n", qe->q1);
	  qual_edge_hashset_update_hashes(ecr_qual(qe->q1)->u.var.ub);

#if USE_QUAL_EDGE_MEMPOOL
      // TODO: deallocate the duplicate edges
#endif
        } else {
          // printf("## unify_qual_en: NOT updating hashes for %p\n", qe->q1);
        }
      }
      scan_qual_edge_hashset(qe, qess, new_ecr->u.var.ub) {
	if (!eq_qual(new_ecr, qe->q2)) {
          // printf("## unify_qual_en: updating hashes for %p\n", qe->q2);
	  qual_edge_hashset_update_hashes(ecr_qual(qe->q2)->u.var.lb);
        } else {
          // printf("## unify_qual_en: NOT updating hashes for %p\n", qe->q2);
        }
      }
      qual_edge_hashset_update_hashes(new_ecr->u.var.lb);
      qual_edge_hashset_update_hashes(new_ecr->u.var.ub);

      // DEBUG_CHECK_HASH_TABLE(new_ecr->u.var.lb);
      // DEBUG_CHECK_HASH_TABLE(new_ecr->u.var.ub);

      // DEBUG_CHECK_HASH_TABLE(new_link->u.var.lb);
      // DEBUG_CHECK_HASH_TABLE(new_link->u.var.ub);

      // {
      //   qual q;
      //   qual_set_scanner qss;

      //   scan_qual_set(q, qss, all_vars)
      //   {
      //     q = ecr_qual(q);
      //     DEBUG_CHECK_HASH_TABLE(q->u.var.lb);
      //     DEBUG_CHECK_HASH_TABLE(q->u.var.ub);

      //     // extern int DEBUG_UPDATE_HASHES;
      //     // DEBUG_UPDATE_HASHES = 1;
      //     // qual_edge_hashset_update_hashes(q->u.var.lb);
      //     // qual_edge_hashset_update_hashes(q->u.var.ub);
      //     // DEBUG_UPDATE_HASHES = 0;
      //   }
      // }
      DEBUG_CHECK_ALL_EDGE_SETS();
    }
  DEBUG_CHECK_ALL_VARS();
}

/* Copy the constraint described by e to the identical constraint
   between q1 and q2. */
void copy_constraint (qual_edge e, qual q1, qual q2)
{
  int edgeNumber = -1;
#if DEBUG_EDGE_NUMBER
  edgeNumber = e->edgeNumber;
#endif

  if (constant_qual (q1) || constant_qual (q2))
    mkleq_qual(e->loc, e->loc_index, &e->qgate, q1, q2, NULL);
  if (e->summary_time == 0)
    mkqual_edge (e->loc, e->loc_index, e->kind, 0, q1, q2, &e->qgate, e->u.error_message, NULL, edgeNumber);
  else
    mkqual_edge (e->loc, e->loc_index, e->kind, 0, q1, q2, &e->qgate, NULL, NULL, edgeNumber);
}

/**************************************************************************
 *                                                                        *
 * Solve constraints: CFL reachability                                    *
 *                                                                        *
 **************************************************************************/

static bool is_bound_compatible (qual_edge qe, qual_reason r, qual c, q_direction d)
{
  return ! nonprop_qual (c) && qual_gate_passes_qual (&qe->qgate, c, d);
}

static bool new_summary (location loc, int loc_index,
                         qual q1, qual q2,
			 qual_edge elb, qual_edge eub,
			 qual_gate *path_qgate,
			 int summary_length)
{
  qual_edge se = NULL;
  bool result;
  qual_gate qgate = *path_qgate;

  if (elb && ! compose_qual_gates (&elb->qgate, &qgate, &qgate))
    return FALSE;
  if (eub && ! compose_qual_gates (&qgate, &eub->qgate, &qgate))
    return FALSE;

  q1 = ecr_qual (q1);
  q2 = ecr_qual (q2);

  result = mkqual_edge (loc, loc_index, ek_flow, current_summary_time,
			q1, q2, &qgate, NULL, &se, -1);
  if (result) {
    // quarl 2006-07-12
    //    I don't see how se->summary_time could ever not be
    //    current_summary_time for a new edge, so I'm making it an assertion
    //    instead of a conditional.
    assert(se->summary_time == current_summary_time);
    se->u.summary_length = summary_length;
    current_summary_time++;
    assert(current_summary_time > 0); // overflow?
    // this can be different from current_summary_time-1 in case of reset.
    qual_stats.summary_edges_created++;
  }

  DEBUG_CHECK_ALL_EDGE_SETS();

#if DEBUG_NEW_SUMMARY
 {
   printf ("SUMMARY: %s --> %s\n",
           name_qual(q1),
	   name_qual(q2));

   if (qual_stats.summary_edges_created % 100 == 0)
     printf ("num_sums: %d\n", qual_stats.summary_edges_created);
 }
#endif

  if (result)
    DEBUG_CHECK_SUMMARY_LENGTH(se);
  return result;
}

static bool new_summaries (qual q1, qual q2, qual_gate* qgate, int summary_length)
{
  qual_edge_hashset_scanner lbscan,ubscan;
  qual_edge elb, eub;
  bool result = FALSE;

  q1 = ecr_qual(q1);
  q2 = ecr_qual(q2);

#if DEBUG_NEW_SUMMARY
  /*
  printf ("SEEKING SUMMARIES: %s -(%s;%s->%s)-> %s\n",
	  name_qual (q1),
	  q_direction_to_string (tf->q1 ? tf->d : d_fwd),
	  tf->q1 ? name_qual(tf->q1) : "*",
	  tf->q1 ? name_qual(tf->q2) : "*",
	  name_qual(q2));
  */
#endif

  if (!global_qual(q1) && !global_qual(q2))
    {
      scan_qual_edge_hashset(elb,lbscan,q1->u.var.lb)
	if (elb->kind == ek_open)
	  scan_qual_edge_hashset(eub,ubscan,q2->u.var.ub)
	    if (eub->kind == ek_close && eub->loc_index == elb->loc_index)
	      result |= new_summary (elb->loc, elb->loc_index,
                                     elb->q1, eub->q2, elb, eub, qgate, summary_length + 2);
    }

  if (global_qual(q1))
    scan_qual_edge_hashset(eub,ubscan,q2->u.var.ub)
      if (eub->kind == ek_close)
	result |= new_summary (q1->u.var.loc, q1->u.var.loc_index,
                               q1, eub->q2, NULL, eub, qgate, summary_length + 1);

  if (global_qual(q2))
    scan_qual_edge_hashset(elb,lbscan,q1->u.var.lb)
      if (elb->kind == ek_open)
	result |= new_summary (elb->loc, elb->loc_index,
                               elb->q1, q2, elb, NULL, qgate, summary_length + 1);

  /* if (global_qual(q1) && global_qual(q2))
     _fwd_new_summary (p, NULL, NULL, q1, q2); */

  return result;
}

// #define QGINDEX(qg) (((qg)->forward ? 2 : 0) + ((qg)->backward ? 1 : 0))
#define QGINDEX(qg) ((qual_gate_get_forward(qg)?2:0) + (qual_gate_get_backward(qg)?1:0))
/* Maps quals --> (qg_0, qg_1, qg_2, qg_3)
   where qg_i has QGINDEX(qg_i) == i */
DEFINE_MAP(qual_gate_visit_map,qual,qual_gate*,hash_qual,eq_qual);

/* Basically maintains 4 seperate reachability records: one for each
   possible QGINDEX.

   Returns TRUE if we already visited q via a path that
   supersedes qgate
*/
static bool record_visit(region r, qual q, qual_gate* qgate,
			 qual_gate_visit_map visited)
{
  qual_gate *old_qgates;
  qual_gate* old_qgate;
  // CHECK_QUAL_GATE(qgate);

  q = ecr_qual(q);

  if (!qual_gate_visit_map_lookup(visited, q, &old_qgates))
    {
      old_qgates = rarrayalloc(r, 4, struct Qual_gate);
      memset(old_qgates, 0, 4 * sizeof(*old_qgates));
      qual_gate_visit_map_insert(visited, q, old_qgates);
      qual_gate_set_forward(&old_qgates[0], FALSE);
      qual_gate_set_backward(&old_qgates[0], FALSE);
      qual_gate_set_forward(&old_qgates[1], FALSE);
      qual_gate_set_backward(&old_qgates[1], TRUE);
      qual_gate_set_forward(&old_qgates[2], TRUE);
      qual_gate_set_backward(&old_qgates[2], FALSE);
      qual_gate_set_forward(&old_qgates[3], TRUE);
      qual_gate_set_backward(&old_qgates[3], TRUE);
    }
  old_qgate = &old_qgates[QGINDEX(qgate)];
  assert (QGINDEX(old_qgate) == QGINDEX(qgate));

  DEBUG_CHECK_QUAL_GATE(old_qgate);
  DEBUG_CHECK_QUAL_GATE(qgate);

  if (qual_gate_covers(old_qgate, qgate))
    return TRUE;

  qual_gate_or(old_qgate->mask, qgate->mask, old_qgate->mask);

  return FALSE;
}

// quarl 2006-07-11
//    Collate the stable recursive args.  _find_bak_summaries and
//    _find_fwd_summaries exhaust 8MB stack space with 100,000s of recursions;
//    this reduces stack usage (and uses less memory in general).  (Use
//    'ulimit -s' in the shell before invoking qualcc to set stack size.)
struct Find_summary_args {
  // TODO: we can get rid of the 'region r' arg since it's only used to add to
  // 'visited' and that already knows its region; hash_table just doesn't
  // expose it (yet).
  region r;
  qual origin;
  qual_gate_visit_map visited;
};

static bool find_bak_summaries (qual q);

static bool _find_bak_summaries (struct Find_summary_args *args,
                                 qual q, qual_gate* qgate,
				 int summary_length)
{
  qual_edge_hashset_scanner qess;
  qual_edge qe;
  bool retval = FALSE;
  bool repeat = TRUE;

  qual_stats._find_bak_summary_depth++;
  if (qual_stats._find_bak_summary_depth > qual_stats._find_bak_summary_depth_max)
    qual_stats._find_bak_summary_depth_max = qual_stats._find_bak_summary_depth;

  // CHECK_QUAL_GATE(qgate);

  q = ecr_qual (q);

  // quarl 2006-07-12
  //    Check q->u.var.bak_summary_dead_end first since it's faster and should
  //    imply that q was already visited, anyway.
  if (q->u.var.bak_summary_dead_end || record_visit(args->r, q, qgate, args->visited))
    goto done;

  if (param_qual (q))
    retval |= new_summaries (q, args->origin, qgate, summary_length);
  else
    q->u.var.bak_summary_dead_end = TRUE;

  if (global_qual(q))
    goto done;

  while (repeat)
    {
      repeat = FALSE;

      scan_qual_edge_hashset (qe, qess, q->u.var.lb)
	{
	  qual_gate new_qgate;
          memset(&new_qgate, 0, sizeof(new_qgate));
	  qual q1 = ecr_qual(qe->q1);

	  if (!compose_qual_gates (&qe->qgate, qgate, &new_qgate))
	    continue;

	  if (qe->kind == ek_flow)
	    {
	      retval |= _find_bak_summaries (args,
                                             q1, &new_qgate,
					     summary_length + edge_length(qe));
	      q->u.var.bak_summary_dead_end &= q1->u.var.bak_summary_dead_end;
	    }
	  else if (qe->kind == ek_close)
	    repeat |= find_bak_summaries (q1);
	}
    }

done:
  qual_stats._find_bak_summary_depth--;
  return retval;
}

static bool find_bak_summaries (qual q)
{
  qual_gate qgate;
  bool result;

  q = ecr_qual (q);

  if (q->u.var.bak_seeded)
    return FALSE;
  q->u.var.bak_seeded = TRUE;

  qual_stats.find_bak_summary_depth++;
  if (qual_stats.find_bak_summary_depth > qual_stats.find_bak_summary_depth_max)
    qual_stats.find_bak_summary_depth_max = qual_stats.find_bak_summary_depth;

  struct Find_summary_args args[1];
  // quarl 2006-07-11
  //    TODO: re-use region from calling _find_bak_summaries, if any?
  args->r = newregion();
  args->origin = q;
  args->visited = make_qual_gate_visit_map(args->r, 32);

  // can this be memcpy(qgate, open_qgate, sizeof(*qgate)) ?
  memset(&qgate, 0, sizeof(qgate)); // zero-fill (esp. unused bits)
  qual_gate_set_all (qgate.mask, TRUE);
  qual_gate_set_backward(&qgate, TRUE);
  qual_gate_set_forward(&qgate, TRUE);

  result = _find_bak_summaries (args, q, &qgate, 0);

  deleteregion(args->r);
  qual_stats.find_bak_summary_depth--;

  return result;
}

static bool find_fwd_summaries (qual q);

static bool _find_fwd_summaries (struct Find_summary_args *args,
                                 qual q, qual_gate* qgate,
				 int summary_length)
{
  qual_edge_hashset_scanner qess;
  qual_edge qe;
  bool retval = FALSE;
  bool repeat = TRUE;

  qual_stats._find_fwd_summary_depth++;
  if (qual_stats._find_fwd_summary_depth > qual_stats._find_fwd_summary_depth_max)
    qual_stats._find_fwd_summary_depth_max = qual_stats._find_fwd_summary_depth;

  q = ecr_qual (q);

  // DEBUG_CHECK_QUAL(q);
  if (q->u.var.fwd_summary_dead_end || record_visit(args->r, q, qgate, args->visited))
    goto done;

  if (ret_qual (q))
    retval |= new_summaries (args->origin, q, qgate, summary_length);
  else
    q->u.var.fwd_summary_dead_end = TRUE;

  if (global_qual(q))
    goto done;

  while (repeat)
    {
      repeat = FALSE;

      scan_qual_edge_hashset (qe, qess, q->u.var.ub)
	{
	  qual_gate new_qgate;
          memset(&new_qgate, 0, sizeof(new_qgate));
	  qual q2 = ecr_qual(qe->q2);

	  if (!compose_qual_gates (qgate, &qe->qgate, &new_qgate))
	    continue;

	  if (qe->kind == ek_flow)
	    {
	      retval |= _find_fwd_summaries (args,
                                             q2, &new_qgate,
					     summary_length + edge_length(qe));
	      q->u.var.fwd_summary_dead_end &= q2->u.var.fwd_summary_dead_end;
	    }
	  else if (qe->kind == ek_open)
	    repeat |= find_fwd_summaries (q2);
	}
    }

done:
  qual_stats._find_fwd_summary_depth--;
  return retval;
}

static bool find_fwd_summaries (qual q)
{
  qual_gate qgate;
  bool result;

  q = ecr_qual (q);

  if (q->u.var.fwd_seeded)
    return FALSE;
  q->u.var.fwd_seeded = TRUE;

  qual_stats.find_fwd_summary_depth++;
  if (qual_stats.find_fwd_summary_depth > qual_stats.find_fwd_summary_depth_max)
    qual_stats.find_fwd_summary_depth_max = qual_stats.find_fwd_summary_depth;

  struct Find_summary_args args[1];
  args->r = newregion();
  args->origin = q;
  args->visited = make_qual_gate_visit_map(args->r, 32);

  // TODO: can this be memcpy(qgate, open_qgate, sizeof(*qgate)) ?
  memset(&qgate, 0, sizeof(qgate)); // zero-fill
  qual_gate_set_all (qgate.mask, TRUE);
  qual_gate_set_backward(&qgate, TRUE);
  qual_gate_set_forward(&qgate, TRUE);

  result = _find_fwd_summaries (args, q, &qgate, 0);

  deleteregion(args->r);
  qual_stats.find_fwd_summary_depth--;

  return result;
}

static void propagate_bounds(qual q, qual_hashset worklist)
{
  qual_edge_hashset_scanner qess;
  qual_edge qe;

  assert(is_ecr_qual(q));

  if (returned_qual (q))
    scan_qual_edge_hashset (qe, qess, q->u.var.lb)
      if (qe->kind == ek_close)
	find_bak_summaries (qe->q1);

  if (passed_qual (q))
    scan_qual_edge_hashset (qe, qess, q->u.var.ub)
      if (qe->kind == ek_open)
	find_fwd_summaries (qe->q2);

  scan_qual_edge_hashset (qe, qess, q->u.var.lb)
    {
      qual_reason_map_scanner qrms;
      qual_reason qr;
      qual c;

      if (qe->kind == ek_close || flag_context_summary)
	continue;

      scan_qual_reason_map (c, qr, qrms, q->u.var.qbounds->ubr)
	if (is_bound_compatible(qe, qr, c, d_bak))
	  {
	    int r = 0;

	    if (qe->kind == ek_flow || qe->kind == ek_open)
	      r = _mkleq_qual (NO_LOCATION, 0, qe->q1, c, FALSE, qr->ci_bound,
			       &qe->qgate, NULL, NULL, qe);
	    else if (flag_context_summary)
	      /* demote to a context-summary bound */
	      r = _mkleq_qual (NO_LOCATION, 0, qe->q1, c, FALSE, FALSE,
			       &qe->qgate, NULL, NULL, qe);

	    if (worklist && (INSERTED_LOWER(r) || INSERTED_UPPER(r)))
	      qual_hashset_insert (&worklist, ecr_qual (qe->q1));

	  }
    }


  scan_qual_edge_hashset (qe, qess, q->u.var.ub)
    {
      qual_reason_map_scanner qrms;
      qual_reason qr;
      qual c;

      if (qe->kind == ek_open || flag_context_summary)
	continue;

      scan_qual_reason_map (c, qr, qrms, q->u.var.qbounds->lbr)
	if (is_bound_compatible(qe, qr, c, d_fwd))
	  {
	    int r = 0;

	    if (qe->kind == ek_flow || qe->kind == ek_close)
	      r = _mkleq_qual (NO_LOCATION, 0, c, qe->q2, FALSE, qr->ci_bound,
			       &qe->qgate, NULL, NULL, qe);
	    else if (flag_context_summary)
	      /* demote to a context-summary bound */
	      r = _mkleq_qual (NO_LOCATION, 0, c, qe->q2, FALSE, FALSE,
			       &qe->qgate, NULL, NULL, qe);

	    if (worklist && (INSERTED_LOWER(r) || INSERTED_UPPER(r)))
	      qual_hashset_insert (&worklist, ecr_qual(qe->q2));
	  }
    }
}

static void cfl_flow (qual_set vars)
{
  // region scratch;
  // region newscratch;
  qual_hashset pf_worklist;
  qual_hashset newpf_worklist;
  qual_hashset_scanner qhss;
  qual_set_scanner qss;
  qual q;

  // scratch = newregion();
  // pf_worklist = empty_qual_hashset(scratch);
  pf_worklist = empty_qual_hashset(NULL);
  scan_qual_set (q, qss, vars)
    if (q == ecr_qual(q) && q->u.var.qbounds)
      qual_hashset_insert (&pf_worklist, q);

  while (qual_hashset_size(pf_worklist) > 0)
    {
      // newscratch = newregion();
      // newpf_worklist = empty_qual_hashset(newscratch);
      newpf_worklist = empty_qual_hashset(NULL);

      scan_qual_hashset (q, qhss, pf_worklist)
	propagate_bounds(q, newpf_worklist);

      destroy_qual_hashset(pf_worklist);
      pf_worklist = newpf_worklist;
      // deleteregion (scratch);
      // scratch = newscratch;
    }

  // deleteregion(scratch);
  destroy_qual_hashset(pf_worklist);
}


#if 0

#define NBLOBS 10

static qual find_blob (qual_set vars)
{
  qual blobs[NBLOBS];
  int blob_scores[NBLOBS];
  qual_set_scanner scan;
  qual temp;
  int i;
  qual_edge_set_scanner pred_scanner;
  qual_edge pred;

  memset (blobs, 0, sizeof (blobs));
  memset (blob_scores, 0, sizeof (blob_scores));

  scan_qual_set (temp, scan, vars)
    {
      if (temp->kind == qk_variable)
	{
	  int this_blob_score = 0;

	  scan_qual_edge_set (pred, pred_scanner, temp->u.var.lb)
	    {
	       /* if (pred->kind == ek_flow) */
	      this_blob_score++;
	    }

	  for (i = 0; i < NBLOBS; i++)
	    {
	      if ((blobs[i] == NULL) || (this_blob_score > blob_scores[i]) ||
		  ((this_blob_score == blob_scores[i]) &&
		   (strcmp (temp->u.var.name, blobs[i]->u.var.name) >= 0)))
		{
		  memmove (blobs + i + 1, blobs + i,
			   sizeof (blobs[0]) * (NBLOBS - i - 1));
		  memmove (blob_scores + i + 1, blob_scores + i,
			   sizeof (blob_scores[0]) * (NBLOBS - i - 1));
		  blobs[i] = temp;
		  blob_scores[i] = this_blob_score;
		  break;
		}
	    }
	}
    }

  for (i = 0; i < NBLOBS && blobs[i]; i++)
    {
      printf ("blob[%d] name: %s global: %d score: %d size: %d",
	      i,
	      blobs[i]->u.var.name,
	      blobs[i]->u.var.global,
	      blob_scores[i],
	      qual_edge_set_size (blobs[i]->u.var.lb));
      printf ("\n");
    }
  return blobs[0];
}
#endif

/***********
 * A fast monomorphic pass to speed up the CFL reachability pass
 ***********/

/* A recursive depth-first-search
   helper function */
static void fiv_dfs (qual q, qual_set* conn_comp, qual_set* cc_cbounds)
{
  qual_edge_hashset_scanner qess;
  qual_edge e;
  qual_bounds qb = NULL;
  qual_reason_map_scanner qrms;
  qual c;
  qual_reason qr;

  /* If this is not a variable (should never happen) or has
     already been visited, stop. */
  q = ecr_qual (q);
  if (q->kind != qk_variable || q->u.var.visited)
    return;

  /* Add this variable to the current connected component */
  qual_set_insert_nocheck (quals_permanent_region, conn_comp, q);

  /* Now add all the consant bounds on this variable to the set of
     constant bounds on this connected component */
  qb = q->u.var.qbounds;
  if (qb)
    {
      scan_qual_reason_map (c, qr, qrms, qb->lbr)
	{
	  if (qr->ci_bound)
	    {
	      assert (c->kind == qk_constant);
	      qual_set_insert (quals_permanent_region, cc_cbounds, c);
	    }
	}
      scan_qual_reason_map (c, qr, qrms, qb->ubr)
	{
	  if (qr->ci_bound)
	    {
	      assert (c->kind == qk_constant);
	      qual_set_insert (quals_permanent_region, cc_cbounds, c);
	    }
	}
    }

  /* Mark this variable as visited */
  q->u.var.visited = TRUE;

  /* Visit its neighbors */
  scan_qual_edge_hashset (e, qess, q->u.var.lb)
    {
      fiv_dfs (e->q1, conn_comp, cc_cbounds);
    }
  scan_qual_edge_hashset (e, qess, q->u.var.ub)
    {
      fiv_dfs (e->q2, conn_comp, cc_cbounds);
    }
}

/* Return whether this set of constant qualifiers
     contains two qualifiers from the same poset */
static int fiv_could_have_type_error (qual_set cbounds)
{
  po_set_scanner pss;
  po_info po;
  qual_set_scanner qss;
  qual c;
  int count;

  scan_po_set (po, pss, all_pos)
    {
      count = 0;
      scan_qual_set (c, qss, cbounds)
	{
	  assert (c->kind == qk_constant);
	  if (c->u.elt.po == po)
	    {
	      count++;
	      if (count >= 2)
		return TRUE;
	    }
	}
    }
  return FALSE;
}

static qual_set find_important_vars (qual_set vars)
{
  qual_set result;
  qual_set_scanner qss;
  qual start;
  qual_set conn_comp;
  qual_set cc_cbounds;

  result = empty_qual_set (quals_permanent_region);

  reset_visited (vars);

  /* printf ("all_vars: %d nodes\n", qual_set_size (all_vars)); */

  scan_qual_set (start, qss, vars)
    {
      if (start->kind == qk_variable &&
	  start->u.var.visited == FALSE)
	{
	  /* We've found a node in a new connected component, so
	  compute all nodes in this connected component and all the
	  constant bounds on variables in this c.c. */
	  conn_comp = empty_qual_set(quals_permanent_region);
	  cc_cbounds = empty_qual_set(quals_permanent_region);
	  fiv_dfs (start, &conn_comp, &cc_cbounds);

	  if (fiv_could_have_type_error (cc_cbounds))
	    {
	      /* printf ("coco: %d nodes\n", qual_set_size (conn_comp)); */
	      result = qual_set_union_nocheck (result, conn_comp);
	    }
	}
    }

  return result;
}

/**************************************************************************
 *                                                                        *
 * Error reporting                                                        *
 *                                                                        *
 **************************************************************************/

struct error_path_expander_arg {
  region r;
  qual_set s;
  qual source;
};

static void error_path_expander_fn(qual left, qual right, bool bidi,
                                   location loc, int loc_index,
				   polarity p, bool first, void *arg)
{
  struct error_path_expander_arg * epea = (struct error_path_expander_arg *)arg;
  if (variable_qual(left) && ! eq_qual(epea->source, left))
    qual_set_insert(epea->r, &epea->s, ecr_qual(left));
  if (variable_qual(right) && ! eq_qual(epea->source, right))
    qual_set_insert(epea->r, &epea->s, ecr_qual(right));
}

/* Return the list of qualifiers on the path from start to lbc */
static qual_set expand_error_path_down (region r, qual start, qual lbc, bool same_level)
{
  struct error_path_expander_arg epea = { r, empty_qual_set(r), start };
  start = ecr_qual (start);
  _setup_traverse_reason_path_edges(lbc, error_path_expander_fn, &epea);
  _traverse_reason_path_edges(start, lbc, FALSE, same_level, FALSE, d_fwd);
  return epea.s;
}

static qual_set expand_error_path_up (region r, qual start, qual ubc, bool same_level)
{
  struct error_path_expander_arg epea = { r, empty_qual_set(r), start };
  start = ecr_qual (start);
  _setup_traverse_reason_path_edges(ubc, error_path_expander_fn, &epea);
  _traverse_reason_path_edges(start, ubc, FALSE, same_level, TRUE, d_fwd);
  return epea.s;
}

/* Is this variable untypeable only because it has flow to/from some
   other untypeable variable? */
static bool is_derivative_error (qual e, qual lbc, qual ubc)
{
  qual_set lpath;
  qual_set upath;
  qual t;
  qual_set_scanner qss;
  region scratch;
  bool retval = FALSE;

  scratch = newregion();

  lpath = expand_error_path_down (scratch, e, lbc, FALSE);
  upath = expand_error_path_up   (scratch, e, ubc, FALSE);

#ifdef DEBUG
  printf ("is_derivative_error: %s\n",e ->u.var.name);
  dump_qual_set (lpath);
  dump_qual_set (upath);
#endif

  scan_qual_set (t, qss, lpath)
    {
      if (!t->u.var.anonymous && ! eq_qual (t, e) && qual_set_member (upath, t))
	{
	  retval = TRUE;
	  break;
	}
    }

  deleteregion (scratch);

  return retval;
}

static bool is_redundant_error(qual q, qual lbc, qual ubc)
{
  qual_set lpath;
  qual_set upath;
  qual t;
  qual_set_scanner qss;
  region scratch;
  bool retval = FALSE;

  scratch = newregion();

  lpath = expand_error_path_down (scratch, q, lbc, TRUE);
  upath = expand_error_path_up   (scratch, q, ubc, TRUE);

  scan_qual_set (t, qss, lpath)
    if (t->u.var.visited)
      {
	retval = TRUE;
	break;
      }

  scan_qual_set (t, qss, upath)
    if (t->u.var.visited)
      {
	retval = TRUE;
	break;
      }

  deleteregion (scratch);

  return retval;
}

static void update_redundant_errors(qual q, qual lbc, qual ubc)
{
  qual_set lpath;
  qual_set upath;
  qual t;
  qual_set_scanner qss;
  region scratch;

  scratch = newregion();

  lpath = expand_error_path_down (scratch, q, lbc, TRUE);
  upath = expand_error_path_up   (scratch, q, ubc, TRUE);

  scan_qual_set (t, qss, lpath)
    t->u.var.visited = TRUE;

  scan_qual_set (t, qss, upath)
    t->u.var.visited = TRUE;

  deleteregion (scratch);
}

#if 0
static void qual_error_complexity_traversal_fn(qual left, qual right, bool bidi,
                                               location loc, int loc_index,
					       polarity p, bool first, void *arg)
{
  int *complexity = (int *)arg;
  if (*complexity == 0 && first)
    *complexity += 5;
  (*complexity)++;
}
#endif

/* A Heuristic: The complexity of an error is the length of the paths
   to each bound */
static int qual_error_complexity(qual q, qual_error qe)
{
  qual_reason qr;
  int lcomplexity = 0;
  int ucomplexity = 0;

  insist(qual_reason_map_lookup(q->u.var.qbounds->lbr, qe->lbc, &qr));
  if (qr->kind == rk_edge)
    lcomplexity = qr->u.e.pathlength;

  insist(qual_reason_map_lookup(q->u.var.qbounds->ubr, qe->ubc, &qr));
  if (qr->kind == rk_edge)
    ucomplexity = qr->u.e.pathlength;

  return lcomplexity + ucomplexity;
}

/* Print an appropriate user error if one exists.
   Return TRUE if we printed one, false otherwise. */
static bool print_user_error_message(qual q, qual_error qe)
{
  qual_reason qr;
  qual_edge e;
  region scratch;
  bool result = FALSE;

  assert (variable_qual(q));
  scratch = newregion();

  /* First try to print an error message on the lower bound edge */
  assert(qual_reason_map_lookup(q->u.var.qbounds->lbr, qe->lbc, &qr));
  if (qr->kind == rk_edge)
    {
      /* Get the originial edge, not summary edges */
      e = qr->u.e.qe;
      while (e->summary_time != 0)
	{
	  qual_edge_list qel;

	  if (eq_qual(e->q1, e->q2))
	    break;

	  qel = find_summary_path(scratch, e);
	  e = qual_edge_list_get_tail(qel);
	}

      /* If this edge has a user-supplied error message, print it. */
      if (!e->summary_time && e->u.error_message)
	{
	  report_qual_error(e->loc, sev_err, q, e->u.error_message);
	  result = TRUE;
	}
    }

  /* If we didn't find a user-defined error message above, then try the
     exact same thing on the upper bound edge. */
  if (!result)
    {
      assert(qual_reason_map_lookup(q->u.var.qbounds->ubr, qe->ubc, &qr));
      if (qr->kind == rk_edge)
	{
	  e = qr->u.e.qe;
	  while (e->summary_time != 0)
	    {
	      qual_edge_list qel;

	      if (eq_qual(e->q1, e->q2))
		break;

	      qel = find_summary_path(scratch, e);
	      e = qual_edge_list_head(qel);
	    }

	  if (!e->summary_time && e->u.error_message)
	    {
	      report_qual_error(e->loc, sev_err, q, e->u.error_message);
	      result = TRUE;
	    }
	}
    }

  deleteregion(scratch);

  return result;
}

/* This finds the length of the longest filename mentioned in this error */
static void explain_error_find_max_filename_len(qual left, qual right, bool bidi,
						location loc, int loc_index,
                                                polarity p,
						bool first,void *arg)
{
  int *maxlen = (int *)arg;
  if (loc!=NO_LOCATION)
    {
      int l = strlen(location_filename(loc));
      *maxlen = l > *maxlen ? l : *maxlen;
    }
}

/* This function is cribbed from pam_print_edge */
static void explain_error_print_edge(qual left, qual right, bool bidi,
                                     location loc, int loc_index,
                                     polarity p,
				     bool first, void *arg)
{
  const char *s, *arrow = NULL;
  static int left_size;
  static qual last_right = NULL;
//   int * maxfilenamelen = (int *)arg;
  qual to_print;

  arrow = bidi ? "==" : "<=";

  if (loc!=NO_LOCATION) {
    // dsw: this doesn't work with C-x backtic in emacs
//     fprintf (stderr, "%*s:%-6ld ",
// 	     *maxfilenamelen, location_filename(loc),
// 	     location_line_number(loc));
    // we provide the column number if it is available
    unsigned long loc_column = location_column_number(loc);
    if (loc_column == -1) {
      // no column information
      fprintf(stderr, "%s:%ld: ",
              location_filename(loc),
              location_line_number(loc));
    } else {
      // we have column information
      fprintf(stderr, "%s:%ld:%ld: ",
              location_filename(loc),
              location_line_number(loc),
              loc_column);
    }
  }

  if (first) {
    qual init_qual = left;
    to_print = right;
    s = unique_name_qual(init_qual);
    left_size = fprintf(stderr, "%s", name_qual(init_qual));
#ifdef DEBUG
    /* Was if (flag_ugly) */
    left_size += fprintf(stderr, " %s", s);
#endif
    left_size += fprintf(stderr, " ");
  } else {
//     fprintf(stderr, "%*s", left_size, "");
    to_print = right;
  }

  fprintf(stderr, "%s ", arrow);

  fprintf(stderr, "%s", name_qual(to_print));
#ifdef DEBUG
  /* Was if (flag_ugly) */
    s = unique_name_qual(to_print);
    fprintf(stderr, " %s", s);
#endif
    fprintf(stderr, "\n");

  last_right = right;
}



static void explain_error_print_qual(qual q, void *arg)
{
  fdprintf_fd = stderr;
  print_qual_raw(fdprintf, q, &open_qgate);
  fprintf(stderr, " ");
}

#if DELAY_SET_NAMES
static void declare_qvar_need_name_1(qual q)
{
  assert(is_ecr_qual(q));
  switch (q->kind)
  {
  case qk_constant: break;
  case qk_variable:
    declare_qvar_need_name(q);
    break;
  default:
    assert(0);
  }
}

static void declare_all_qvars_need_names()
{
  qual_set_scanner qss;
  qual q;
  scan_qual_set(q, qss, all_vars) {
    if (!is_ecr_qual(q)) continue;
    declare_qvar_need_name_1(q);
  }
}

static void declare_error_path_need_names_1(qual left, qual right, bool bidi,
                                            location loc, int loc_index,
                                            polarity p,
                                            bool first, void *arg)
{
  if (first) declare_qvar_need_name_1(left);
  declare_qvar_need_name_1(right);
}

static void declare_error_path_need_names(qual q, qual_error qe)
{
  traverse_error_path_edges(q,TRUE,FALSE,&declare_error_path_need_names_1,NULL);
}
#endif

static void explain_error(qual q, qual_error qe)
{
  int maxfilenamelen = 0;

  /* Print the qualifier bounds */
  fprintf(stderr, "%s: ", name_qual(q));
#ifdef DEBUG
  /* Was if (flag_ugly) */
    fprintf(stderr, "%s: ", unique_name_qual(q));
#endif
  scan_qual_bounds_sorted(q, &open_qgate, explain_error_print_qual, NULL);
  fprintf(stderr, "\n");

  DEBUG_CHECK_ALL_EDGE_SETS();

  /* Print the path(s) to the bounds */

  /* First, find the length of the longest filename in the error path, for formatting */
  traverse_error_path_edges(q,TRUE,FALSE,explain_error_find_max_filename_len,&maxfilenamelen);
  /* Now print the error path */
  traverse_error_path_edges(q,TRUE,FALSE,explain_error_print_edge,&maxfilenamelen);
  fprintf(stderr, "\n");
}

#define NUM_ERROR_PRIORITIES 60
#define FOUND_ERROR 0x01
#define PRINTED_ERROR 0x02
#define MISSED_ERRORS(x) ((x) == FOUND_ERROR)

#if DELAY_SET_NAMES
static void declare_error_path_need_names(qual q, qual_error qe);
#endif

struct Qual_error_descriptor {
  qual q;
  qual_error qe;
};
typedef struct Qual_error_descriptor *qual_error_descriptor;

DEFINE_LIST(qual_error_descriptor_list,qual_error_descriptor);
struct ErrorReporter {
  qual_list prioritized_errors[NUM_ERROR_PRIORITIES];
  qual_error_descriptor_list filtered_errors;
  int error_count;
  int filtered_error_count;
  bool anonymous;
};

static void ErrorReporter_bucket_errors(struct ErrorReporter *reporter, qual_set vars, region r)
{
  qual_set_scanner qss;
  qual q;
  /* Scan the errors, and bucket them by how hard they are to explain. */
  scan_qual_set (q, qss, vars)
    {
      /* For each variable with upper or lower bounds */
      if (is_ecr_qual(q) && q->kind == qk_variable && q->u.var.qbounds)
	{
	  qual_error_list qel = new_qual_error_list (r);

	  /* If the variable has inconsistent bounds */
	  if (! _may_leq_qual (q, q, FALSE, &qel, r))
	    {
	      qual_error_list_scanner qels;
	      qual_error qe = NULL;
	      int min_complexity = NUM_ERROR_PRIORITIES-1;

	      /* Find the lowest complexity error on this variable */
	      qual_error_list_scan (qel, &qels);
	      while (qual_error_list_next (&qels, &qe))
		{
		  int complexity = qual_error_complexity(q, qe);
		  if (complexity < min_complexity)
		    min_complexity = complexity;
		}
	      /* Sometimes the summary paths are super long. So cap it.
		 This may be a bug. */
	      if (min_complexity < 0)
		min_complexity = NUM_ERROR_PRIORITIES-1;

	      /* Add this qual_error_descriptor to the appropriate bucket */
	      reporter->prioritized_errors[min_complexity] =
		qual_list_cons(r, reporter->prioritized_errors[min_complexity], q);
              reporter->error_count += 1;
	    }
	}
    }
}

static void ErrorReporter_collect_errors(struct ErrorReporter *reporter, region r)
{
  int i;

  for (i = 0; i < NUM_ERROR_PRIORITIES; i++)
    {
      qual_list_scanner qls;
      qual q = NULL;
      qual_list_scan(reporter->prioritized_errors[i], &qls);
      while (qual_list_next(&qls, &q))
	{
	  if (!q->u.var.anonymous || reporter->anonymous)
	    {
              qual_error_list_scanner qels;
              qual_error qe = NULL;

              /* Get the error paths for this variable */
              qual_error_list qel = new_qual_error_list (r);
              _may_leq_qual (q, q, FALSE, &qel, r);

	      /* If any of the errors on the variable are original to that
		 variable. */
	      qual_error_list_scan (qel, &qels);
	      while (qual_error_list_next (&qels, &qe))
		{
		  if (flag_all_errors ||
		      (!is_derivative_error (q, qe->lbc, qe->ubc) &&
		       !is_redundant_error (q, qe->lbc, qe->ubc)))
		    {
                      if (flag_max_errors && reporter->filtered_error_count >= flag_max_errors) {
                        fprintf(stderr, "WARNING: More than %d warnings; subsequent warnings omitted.\n",
                                flag_max_errors);
                        return;
                      }

                      qual_error_descriptor qed = ralloc(r, struct Qual_error_descriptor);
                      qed->q = q;
                      qed->qe = qe;

                      reporter->filtered_errors =
                        qual_error_descriptor_list_cons(r, reporter->filtered_errors, qed);

                      reporter->filtered_error_count += 1;

		      /* Update our redundant error supression state. */
		      update_redundant_errors(q, qe->lbc, qe->ubc);

		      break;
		    }
		}
	    }
	}
    }
}

static void ErrorReporter_print_errors(struct ErrorReporter *reporter)
{
  qual_error_descriptor_list_scanner qedls;
  qual_error_descriptor qed = NULL;
  int i;

#if DELAY_SET_NAMES
  i = 0;
  qual_error_descriptor_list_scan(reporter->filtered_errors, &qedls);
  while (qual_error_descriptor_list_next(&qedls, &qed)) {
    ++i;
    declare_qvar_need_name(qed->q);
    if (flag_explain_errors) {
      declare_error_path_need_names(qed->q, qed->qe);
    }
  }
  assign_qvar_names();
#endif

  fprintf(stderr, "Reporting %d of %d errors (%s)\n",
          reporter->filtered_error_count,
          reporter->error_count,
          reporter->anonymous ? "including anonymous" : "excluding anonymous");

  i = 0;
  qual_error_descriptor_list_scan(reporter->filtered_errors, &qedls);
  while (qual_error_descriptor_list_next(&qedls, &qed)) {
    /* Try to print a good error message, but
       fallback on a default if necessary. */
    ++i;
    if (!print_user_error_message(qed->q, qed->qe)) {
      report_qual_error(qed->q->u.var.loc, sev_err, qed->q,
                        "WARNING (%d of %d): %s treated as %s and %s\n",
                        i, reporter->filtered_error_count,
                        name_qual (qed->q),
                        name_qual (qed->qe->lbc),
                        name_qual (qed->qe->ubc));
    }

    if (flag_explain_errors)
      explain_error(qed->q, qed->qe);
  }

}

/* Return a boolean: 0 = no type errors, 1 = we found type errors */
static int report_errors(qual_set vars)
{
  DEBUG_CHECK_ALL_EDGE_SETS();

  region scratch = newregion();

  struct ErrorReporter reporter[1];
  int i;
  for (i = 0; i < NUM_ERROR_PRIORITIES; i++)
    reporter->prioritized_errors[i] = new_qual_error_descriptor_list(scratch);
  reporter->filtered_errors = new_qual_error_descriptor_list(scratch);
  reporter->error_count = 0;
  reporter->filtered_error_count = 0;
  reporter->anonymous = FALSE;

  reset_visited(vars);

  ErrorReporter_bucket_errors(reporter, vars, scratch);

  if (reporter->error_count == 0) {
    return FALSE;               // no type errors
  }

  if (!flag_all_errors) {
    /* First try printing errors, but don't print errors on anonymous
       variables */
    ErrorReporter_collect_errors(reporter, scratch);
  }

  /* If that screwed up (i.e. there were errors but we didn't print
     any), then try again, printing the errors on anonymous variables
     this time. This should happen very rarely, and suppressing the
     anonymous variable errors drastically reduces the extraneous
     errors reported, so this is definitely worth while. */
  if (reporter->filtered_error_count == 0) {
    reporter->anonymous = TRUE;
    ErrorReporter_collect_errors(reporter, scratch);
  }
  assert(reporter->filtered_error_count != 0);

  ErrorReporter_print_errors(reporter);

  deleteregion(scratch);
  reset_visited(vars);
  return TRUE;                  // there were type errors
}

/**************************************************************************
 *                                                                        *
 * Misc. s18n code                                                        *
 *                                                                        *
 **************************************************************************/

static void reset_cfl_flags(qual_set vars)
{
  qual_set_scanner qss;
  qual q;

  scan_qual_set(q, qss, vars)
    {
      q->u.var.fwd_seeded = FALSE;
      q->u.var.bak_seeded = FALSE;
      q->u.var.fwd_fs_seeded = FALSE;
      q->u.var.bak_fs_seeded = FALSE;
      q->u.var.fwd_summary_dead_end = FALSE;
      q->u.var.bak_summary_dead_end = FALSE;
    }
}

/* Save quals graph */
int serialize_quals_graph(s18n_context *sc)
{
  DEBUG_CHECK_ALL_EDGE_SETS();
  if (s18n_serialize(sc, &qual_set_type, all_vars) < 0)
    fail("Error while writing output file");
  return 0;
}

void merge_quals_graph(s18n_context *sc)
{
  qual_set new_vars;

  DEBUG_CHECK_ALL_EDGE_SETS();

  if (s18n_deserialize(sc, &qual_set_type, &new_vars) < 0)
    fail("Invalid .q file\n");

  // {
  //   qual_set_scanner qss;
  //   qual q;

  //   scan_qual_set(q, qss, new_vars) {
  //     fprintf(stderr, "## merge_quals_graph(): new var %p. var=%d, is_ecr=%d, lb=%p\n",
  //             q,
  //             (q->kind==qk_variable),
  //             is_ecr_qual(q),
  //             q->u.var.lb);
  //   }
  // }

  all_vars = qual_set_union_nocheck(all_vars, new_vars);
  // TODO: check that all vars read in are not dead
  need_cfl_reset = TRUE;

  DEBUG_CHECK_ALL_EDGE_SETS();
}

/**************************************************************************
 *                                                                        *
 * Graph compaction                                                       *
 *                                                                        *
 **************************************************************************/

/* DEBUGGING: Verify invariants about qual_edge_hashsets */

void really_check_all_edge_sets(void) {
  really_check_edge_sets(all_vars);
}

static void really_check_edge_sets(qual_set vars)
{
  DEBUG_CHECK_EXTERN_QUAL();

  qual q;
  qual_set_scanner qss;
  region scratch;

  scratch = newregion();

  scan_qual_set(q, qss, vars)
    {
      // DEBUG_CHECK_QUAL_EDGE_HASHSET(q->u.var.ub);

      qual_edge_hashset_scanner qess;
      qual_edge e;
      qual_edge_hashset tmp;

      if (!is_ecr_qual(q))
	continue;

#if !USE_OWNING_QUAL_EDGE_HASH_TABLES
      assert(((hash_table)q->u.var.ub)->r == qual_edge_hashsets_region);
      assert(((hash_table)q->u.var.lb)->r == qual_edge_hashsets_region);
#endif

      DEBUG_CHECK_HASH_TABLE(q->u.var.ub);
      DEBUG_CHECK_HASH_TABLE(q->u.var.lb);

      DEBUG_CHECK_HASHSET(q->u.var.ub);
      DEBUG_CHECK_HASHSET(q->u.var.lb);

      // tmp = empty_qual_edge_hashset(scratch);
      tmp = empty_qual_edge_hashset(NULL);

      scan_qual_edge_hashset(e, qess, q->u.var.lb)
	{
          // assert(e->loc_index <= MAX_LOC_INDEX);
	  qual eq = ecr_qual(e->q1);
	  assert(qual_edge_hashset_hash_search(q->u.var.lb, eq_qual_edge_semantic,
					       e, NULL));
	  assert(qual_edge_hashset_hash_search(eq->u.var.ub, eq_qual_edge_semantic,
					       e, NULL));
	  qual_edge_hashset_insert(&tmp, e);

	  if (e->summary_time)
	    assert(find_summary_path(scratch, e));
	}

      assert(qual_edge_hashset_size(tmp) == qual_edge_hashset_size(q->u.var.lb) && "8f3d4600-5d12-415c-b73e-d53db5eaa150");

      // tmp = empty_qual_edge_hashset(scratch);
      qual_edge_hashset_reset(tmp);

      scan_qual_edge_hashset(e, qess, q->u.var.ub)
	{
	  qual eq = ecr_qual(e->q2);
	  assert(qual_edge_hashset_hash_search(q->u.var.ub, eq_qual_edge_semantic,
					       e, NULL));
	  assert(qual_edge_hashset_hash_search(eq->u.var.lb, eq_qual_edge_semantic,
					       e, NULL));
	  qual_edge_hashset_insert(&tmp, e);

	  if (e->summary_time)
	    assert(find_summary_path(scratch, e));
	}

      assert(qual_edge_hashset_size(tmp) == qual_edge_hashset_size(q->u.var.ub) && "bc09f538-251a-44c1-9842-3ed6ec11e6d7");
      destroy_qual_edge_hashset(tmp);
    }

  deleteregion(scratch);
}

/********
 * Local transformations
 ********/

#define GLOB_NO     1
#define GLOB_YES    2
#define GLOB_ANY    3

#define EK_FLOW  1
#define EK_OPEN  2
#define EK_CLOSE 4
#define EK_ANY   7

#define EQIND_YES    1
#define EQIND_NO     2
#define EQIND_ANY    3

#define TRANS_NONE  0
#define TRANS_EDGE  1
#define TRANS_BLOCK 2

#define NEW_EKIND_E1   0
#define NEW_EKIND_E2   1
#define NEW_EKIND_FLOW 2

struct local_transform
{
  int a_global;
  int b_global;
  int c_global;
  int e1_kind;
  int e2_kind;
  int equal_indices;

  int new_edge_status;
  int new_edge_kind;
};

struct local_transform transforms[] =
  {
    /* A transformation rule states that for edges e1 and e2 giving
       path a --> b --> c, if

       - a's globality matches a_global,
       - b's globality matches b_global,
       - c's globality matches c_global,
       - e1->kind matches e1_kind,
       - e2->kind matches e2_kind,
       - and if the equality of e1->index and e2->index matches
         equal_indices

       then the path (e1, e2)

       - TRANS_NONE: can never occur in any CFL path
       - TRANS_EDGE: is equivalent to a single edge a --> c with (kind, location)
         - (e1->kind, e1->loc)     if new_edge_kind == NEW_EKIND_E1
	 - (e2->kind, e2->loc)     if new_edge_kind == NEW_EKIND_E2
	 - (ek_flow, any location) if new_edge_kind == NEW_EKIND_FLOW
       - TRANS_BLOCK: there is no single edge that is equivalent to the pair (e1, e2)

       If there is no matching entry, then the edge pair is assumed to
       be TRANS_BLOCK.

       The table currently contains no TRANS_BLOCK entries.  As a
       consequence, the table is order-independent.  Please maintain
       this property unless you have a _really_ good reason.
       Otherwise, proving the rules correct becomes very complicated.

       Also, I believe the table captures all possible local
       transformations, so there should be no need for modifications.
    */


    /* Rules that don't require any globals */

    { GLOB_ANY, GLOB_NO,  GLOB_ANY, EK_FLOW,  EK_ANY,   EQIND_ANY, TRANS_EDGE,  NEW_EKIND_E2 },

    { GLOB_ANY, GLOB_NO,  GLOB_ANY, EK_OPEN,  EK_FLOW,  EQIND_ANY, TRANS_EDGE,  NEW_EKIND_E1 },
    { GLOB_ANY, GLOB_NO,  GLOB_ANY, EK_OPEN,  EK_CLOSE, EQIND_YES, TRANS_EDGE,  NEW_EKIND_FLOW },
    { GLOB_ANY, GLOB_NO,  GLOB_ANY, EK_OPEN,  EK_CLOSE, EQIND_NO,  TRANS_NONE,  0 },

    { GLOB_ANY, GLOB_NO,  GLOB_ANY, EK_CLOSE, EK_FLOW,  EQIND_ANY, TRANS_EDGE,  NEW_EKIND_E1 },
    { GLOB_ANY, GLOB_NO,  GLOB_ANY, EK_CLOSE, EK_OPEN,  EQIND_YES, TRANS_EDGE,  NEW_EKIND_FLOW },

    /* Rules that require some nodes to be global */

    { GLOB_YES, GLOB_NO,  GLOB_ANY, EK_CLOSE, EK_ANY,   EQIND_ANY, TRANS_EDGE,  NEW_EKIND_E2 },
    { GLOB_ANY, GLOB_NO,  GLOB_YES, EK_ANY,   EK_OPEN,  EQIND_ANY, TRANS_EDGE,  NEW_EKIND_E1 },

    { GLOB_YES, GLOB_YES, GLOB_ANY, EK_ANY,   EK_ANY,   EQIND_ANY, TRANS_EDGE,  NEW_EKIND_E2 },
    { GLOB_ANY, GLOB_YES, GLOB_YES, EK_ANY,   EK_ANY,   EQIND_ANY, TRANS_EDGE,  NEW_EKIND_E1 },

  };

/* Determine if these two edges can be used to generate a
   transitive edge, and compute the kind of location of that
   transitive edge */
static int compute_composed_kind_and_loc(qual_edge e1, qual_edge e2,
                                         qedge_kind *k,
                                         location *loc, int *loc_index)
{
  if (eq_qual(e1->q1, e1->q2))
    {
      if (e1->kind == ek_flow)
	return TRANS_NONE;
      if (global_qual(e1->q2))
	return TRANS_NONE;
      return TRANS_BLOCK;
    }

  if (eq_qual(e2->q1, e2->q2))
    {
      if (e2->kind == ek_flow)
	return TRANS_NONE;
      if (global_qual(e2->q1))
	return TRANS_NONE;
      return TRANS_BLOCK;
    }

  int i;
  struct local_transform tkind;
  memset(&tkind, 0, sizeof(tkind));
  tkind.a_global = global_qual(e1->q1) ? GLOB_YES : GLOB_NO;
  tkind.b_global = global_qual(e1->q2) ? GLOB_YES : GLOB_NO;
  tkind.c_global = global_qual(e2->q2) ? GLOB_YES : GLOB_NO;
  tkind.e1_kind = e1->kind == ek_flow ? EK_FLOW : e1->kind == ek_open ? EK_OPEN : EK_CLOSE;
  tkind.e2_kind = e2->kind == ek_flow ? EK_FLOW : e2->kind == ek_open ? EK_OPEN : EK_CLOSE;
  tkind.equal_indices = e1->loc_index == e2->loc_index ? EQIND_YES : EQIND_NO;

  for (i = 0; i < sizeof(transforms) / sizeof(transforms[0]); i++)
    if ((tkind.a_global      & transforms[i].a_global) &&
	(tkind.b_global      & transforms[i].b_global) &&
	(tkind.c_global      & transforms[i].c_global) &&
	(tkind.e1_kind       & transforms[i].e1_kind) &&
	(tkind.e2_kind       & transforms[i].e2_kind) &&
	(tkind.equal_indices & transforms[i].equal_indices))
      {
	if (transforms[i].new_edge_status != TRANS_EDGE)
	  return transforms[i].new_edge_status;

	if (transforms[i].new_edge_kind == NEW_EKIND_FLOW)
	  {
	    *k = ek_flow;
	    if (e1->kind != ek_flow) {
	      *loc = e1->loc;
              *loc_index = e2->loc_index;
	    } else {
	      *loc = e2->loc;
              *loc_index = e2->loc_index;
            }
	  }
	else if (transforms[i].new_edge_kind == NEW_EKIND_E1)
	  {
	    *k = e1->kind;
	    *loc = e1->loc;
            *loc_index = e1->loc_index;
	  }
	else if (transforms[i].new_edge_kind == NEW_EKIND_E2)
	  {
	    *k = e2->kind;
	    *loc = e2->loc;
            *loc_index = e2->loc_index;
	  }
	else
	  assert("unknown transform edge kind!");

	return TRANS_EDGE;
      }

  return TRANS_BLOCK;
}

static bool mkcomposed_edge(qual_edge e1, qual_edge e2)
{
  qual_edge e;
  qual_gate qgate;
  memset(&qgate, 0, sizeof(qgate));
  qedge_kind k;
  location loc;
  int loc_index;
  const char *error_message = NULL;

  if (compute_composed_kind_and_loc(e1, e2, &k, &loc, &loc_index) != TRANS_EDGE)
    return FALSE;

  if (!compose_qual_gates(&e1->qgate, &e2->qgate, &qgate))
    return FALSE;

  if (e1->kind != ek_flow && e1->u.error_message)
    error_message = e1->u.error_message;
  else if (e2->kind != ek_flow && e2->u.error_message)
    error_message = e2->u.error_message;
  else if (e1->summary_time == 0 && e1->u.error_message)
    error_message = e1->u.error_message;
  else if (e2->summary_time == 0 && e2->u.error_message)
    error_message = e2->u.error_message;

  // assert(loc_index <= MAX_LOC_INDEX);
  return mkqual_edge(loc, loc_index, k, 0, e1->q1, e2->q2, &qgate, error_message, &e, 0);
}

static bool _induces_new_edge(qedge_kind k, qual q1, qual q2,
                              location loc, int loc_index,
			      qual_gate *qgate)
{
  struct Qual_edge tqe;
  qual_edge old;

  // quarl: operate on ECR (previously a bug!)
  q1 = ecr_qual(q1);
  q1 = ecr_qual(q2);

  memset(&tqe, 0, sizeof(tqe));
  tqe.kind = k;
  tqe.summary_time = 0;
  tqe.q1 = q1;
  tqe.q2 = q2;
  tqe.loc = loc;
  assert(loc_index <= MAX_LOC_INDEX);
  tqe.loc_index = loc == NO_LOCATION ? 0 : loc_index;
  tqe.u.error_message = NULL;
  tqe.qgate = *qgate;

  return !qual_edge_hashset_hash_search(q1->u.var.ub, eq_qual_edge_semantic,
					&tqe, &old);
}

static bool induces_new_edge(qual_edge e1, qual_edge e2)
{
  qual_gate qgate;
  memset(&qgate, 0, sizeof(qgate));
  qedge_kind k;
  location loc;
  int loc_index;

  if (compute_composed_kind_and_loc(e1, e2, &k, &loc, &loc_index) != TRANS_EDGE)
    return FALSE;

  if (!compose_qual_gates(&e1->qgate, &e2->qgate, &qgate))
    return FALSE;

  return _induces_new_edge(k, e1->q1, e2->q2, loc, loc_index, &qgate);
}

#if DEBUG_REJECTIONS
int rejections[16];
#endif

// Returns 0 if removable, non-zero if not removable
//
// If DEBUG_REJECTIONS is enabled, the rejection value is a bitwise-or of all
// rejection reasons; otherwise, it is the first one encountered.  If
// DEBUG_REJECTIONS, then keep track of rejection statistics.
static int check_node_removable(qual q)
{
  qual_edge_hashset_scanner lbs, ubs;
  qual_edge e1, e2;
  int newedge_count;
  qedge_kind k;
  location loc;
  int loc_index;

#if DEBUG_REJECTIONS
  int rejection = 0;
# define REJECT(n) rejection |= n
#else
# define REJECT(n) return n
#endif

  assert(q == ecr_qual(q));
  assert(q->kind == qk_variable);

  if (q->u.var.isextern/* && (global_qual(q) || !q->u.var.defined || q->u.var.qbounds) */)
    REJECT(1);

  int max_induced_edges = (qual_edge_hashset_size(q->u.var.lb) +
                           qual_edge_hashset_size(q->u.var.ub) + 10);

  if (!may_leq_qual(q, q))
    REJECT(4);

  /* Determine if we can delete this node */
  newedge_count = 0;
  scan_qual_edge_hashset(e1, lbs, q->u.var.lb)
    {
      scan_qual_edge_hashset(e2, ubs, q->u.var.ub)
	{
	  if (compute_composed_kind_and_loc(e1, e2, &k, &loc, &loc_index) == TRANS_BLOCK)
	    REJECT(4);
	  else if (induces_new_edge(e1, e2)) {
	    newedge_count++;
            if (newedge_count > max_induced_edges) {
              REJECT(2);
            }
          }
	}
    }

  if (q->u.var.qbounds)
    {
      if (qual_reason_map_size(q->u.var.qbounds->lbr) != 0)
	scan_qual_edge_hashset(e2, ubs, q->u.var.ub)
	  if (e2->kind == ek_open)
            REJECT(8);
      if (qual_reason_map_size(q->u.var.qbounds->ubr) != 0)
	scan_qual_edge_hashset(e1, lbs, q->u.var.lb)
	  if (e1->kind == ek_close)
            REJECT(8);
    }

#undef REJECT
#if DEBUG_REJECTIONS
  if (rejection)
    {
      rejections[rejection]++;
    }
  return rejection;
#else
  return 0;
#endif
}

// returns TRUE if changed.
static int compact_remove_node(qual q)
{
  qual_edge_hashset_scanner lbs, ubs;
  qual_edge e1, e2;
  bool changed = FALSE;

  // DEBUG_CHECK_ALL_QUAL_GATES();

  assert(q == ecr_qual(q));
  assert(q->kind == qk_variable);
  DEBUG_CHECK_VAR(q);

  if (q->u.var.qbounds)
    propagate_bounds(q, NULL);

  // DEBUG_CHECK_ALL_QUAL_GATES();

  DEBUG_CHECK_ALL_EDGE_SETS();

  /* Make transitive edges for all the edge pairs
     (e1, e2) through this node */
  scan_qual_edge_hashset(e1, lbs, q->u.var.lb)
    {
      // DEBUG_CHECK_QUAL_GATE(&e1->qgate);
      scan_qual_edge_hashset(e2, ubs, q->u.var.ub)
	{
          // DEBUG_CHECK_QUAL_GATE(&e2->qgate);
	  changed |= mkcomposed_edge(e1, e2);
          DEBUG_CHECK_QUAL_GATE(&e2->qgate);
	  DEBUG_CHECK_ALL_EDGE_SETS();
	}
      DEBUG_CHECK_QUAL_GATE(&e1->qgate);
    }

  // DEBUG_CHECK_ALL_QUAL_GATES();

  changed |= qual_edge_hashset_size(q->u.var.lb) != 0 ||
    qual_edge_hashset_size(q->u.var.ub) != 0;

  DEBUG_CHECK_ALL_EDGE_SETS();

  scan_qual_edge_hashset(e1, lbs, q->u.var.lb)
    {
      e1->q1 = ecr_qual(e1->q1);
      assert(qual_edge_hashset_hash_search(e1->q1->u.var.ub, eq_qual_edge_semantic, e1, NULL));
    }

  // DEBUG_CHECK_ALL_EDGE_SETS();
  DEBUG_CHECK_VAR(q);
  // CHECK_ALL_QUAL_GATES();
  DEBUG_CHECK_HASH_TABLE(q->u.var.lb);

  /* Delete the edges incident on this node from the
     constraint graph */
  scan_qual_edge_hashset(e1, lbs, q->u.var.lb)
    {
      e1->q1 = ecr_qual(e1->q1);
      // printf("## try_to_remove_node: removing %p: %s - %s\n",
      //        e1, e1->q1->u.var.name, e1->q2->u.var.name);

      assert(e1->q1->kind == qk_variable);
      bool removed = qual_edge_hashset_remove(&e1->q1->u.var.ub, e1);
      assert(removed);
      assert(e1->summary_time == 0);
    }
  qual_edge_hashset_reset(q->u.var.lb);

  DEBUG_CHECK_ALL_EDGE_SETS();
  DEBUG_CHECK_HASH_TABLE(q->u.var.ub);
  // DEBUG_CHECK_ALL_QUAL_GATES();

  scan_qual_edge_hashset(e2, ubs, q->u.var.ub)
    {
      e2->q2 = ecr_qual(e2->q2);
      assert(e2->q2->kind == qk_variable);
      // DEBUG_CHECK_QUAL_GATE(&e2->qgate);
      // DEBUG_CHECK_ALL_QUAL_GATES();
      bool removed = qual_edge_hashset_remove(&e2->q2->u.var.lb, e2);
      DEBUG_CHECK_QUAL_GATE(&e2->qgate);
      // CHECK_ALL_QUAL_GATES();
      assert(removed);
      assert(e2->summary_time == 0);
    }
  // DEBUG_CHECK_ALL_QUAL_GATES();

  qual_edge_hashset_reset(q->u.var.ub);

  DEBUG_CHECK_ALL_EDGE_SETS();

  DEBUG_CHECK_ALL_QUAL_GATES();

  qual_stats.quals_removed_in_compaction++;
  return changed;
}

static void kill_qual(qual q)
{
  // save this so we can destruct its hashtables later.
  qual_set_insert_nocheck(quals_permanent_region, &dead_vars, q);

  q->dead = TRUE;

  // debugging: make sure we don't access the qvar except to check that it's
  // dead.
  //  quarl 2006-05-15
  //     This just doesn't work when 'dead' is a bitfield :(

  // VALGRIND_MAKE_NOACCESS(q, sizeof(*q));
  // VALGRIND_MAKE_READABLE(&q->dead, 1); // this doesn't work for bitfields

  // // mark everything between (q->link, q->u] as noaccess
  // VALGRIND_MAKE_NOACCESS(&q->link + sizeof(q->link),
  //                        (int)&(((qual)0)->u) + sizeof(q->u)
  //                        - ( (int)&(((qual)0)->link) + sizeof(q->link)) );
}

static int is_dead_node(qual q, void *arg)
{
  // this function should only be used as a callback in compact_quals_graph()
  assert(!q->dead);
  qual eq = ecr_qual(q);
  // fprintf(stderr, "## is_dead_node\n");
  // fprintf(stderr, "## is_dead_node q=%p, eq=%p, eq->dead=%d\n",
  //         q, eq, eq->dead);
  if (eq->dead ||
      (!eq->u.var.isextern &&
       qual_edge_hashset_size(eq->u.var.lb) == 0 &&
       qual_edge_hashset_size(eq->u.var.ub) == 0))
  {
    qual_stats.quals_killed++;
    kill_qual(q);
    // fprintf(stderr, "##    result=dead\n\n");
    return 0; /* removable */
  }
  // fprintf(stderr, "##    result=not dead\n\n");
  return 1;
}

// was this qual completely removed from the graph during compaction because
// it was a dead node (no edges)?
bool is_dead_qual(qual q)
{
  return q->dead;
}

/**********
 * Strongly connected component squashing
 **********/

DEFINE_MAP(qual_intptr_t_map,qual,intptr_t,hash_qual,eq_qual);
#define scan_qual_intptr_t_map(kvar, dvar, scanner, map) \
for (qual_intptr_t_map_scan(map, &scanner); \
     qual_intptr_t_map_next(&scanner, &kvar, &dvar);)

static intptr_t cfttime;

static void compute_finish_times_dfs(qual q,
                                     qual_hashset visited,
                                     qual_intptr_t_map finish_times)
{
  // CHECK_ALL_QUAL_GATES();

  // DEBUG_CHECK_ALL_EDGE_SETS();
  qual_edge_hashset_scanner qess;
  qual_edge e;

  q = ecr_qual(q);

  if (qual_hashset_member(visited, q))
    return;
  qual_hashset_insert(&visited, q);

  // CHECK_QUAL_GATE(&open_qgate);

  scan_qual_edge_hashset(e, qess, q->u.var.ub)
    {
      // CHECK_QUAL_GATE(&e->qgate);

      if (e->kind == ek_flow && e->summary_time == 0 &&
	  !memcmp(&e->qgate, &open_qgate, sizeof(e->qgate)))
	compute_finish_times_dfs(e->q2, visited, finish_times);
  // DEBUG_CHECK_ALL_EDGE_SETS();
    }

  qual_intptr_t_map_insert(finish_times, q, cfttime);
  cfttime++;
  // DEBUG_CHECK_ALL_EDGE_SETS();
}

static void compute_finish_times(qual_set vars, qual_intptr_t_map finish_times)
{
  // region scratch;
  qual_hashset visited;
  qual_set_scanner qss;
  qual q;

  // scratch = newregion();
  // visited = empty_qual_hashset(scratch);
  visited = empty_qual_hashset(NULL);

  cfttime = 0;
  scan_qual_set(q, qss, vars)
    compute_finish_times_dfs(q, visited, finish_times);

  // deleteregion(scratch);
  destroy_qual_hashset(visited);
}

static void reverse_reachable_vars(qual q,
				   qual_hashset scc, qual_hashset visited)
{
  qual_edge_hashset_scanner qess;
  qual_edge e;

  q = ecr_qual(q);

  if (qual_hashset_member(visited, q))
    return;
  qual_hashset_insert(&visited, q);

  qual_hashset_insert( &scc, q);

  scan_qual_edge_hashset(e, qess, q->u.var.lb)
    {
      if (e->kind == ek_flow && e->summary_time == 0 &&
	  !memcmp(&e->qgate, &open_qgate, sizeof(e->qgate)))
	reverse_reachable_vars(e->q1, scc, visited);
    }
}

/* Merge strongly connected components of the graph */
static bool merge_sccs(qual_set vars)
{
  qual *ftordered;
  region scratch;
  qual_intptr_t_map finish_times;
  qual_intptr_t_map_scanner qims;
  intptr_t ftime;
  qual q;
  intptr_t i;
  qual_hashset visited;
  qual_hashset_scanner qss;
  bool changed = FALSE;

  scratch = newregion();
  finish_times = make_qual_intptr_t_map(scratch, 16);

  compute_finish_times(vars, finish_times);

  ftordered = rarrayalloc(scratch, cfttime, qual);
  memset(ftordered, 0, cfttime * sizeof(qual));

  scan_qual_intptr_t_map(q, ftime, qims, finish_times)
    {
      assert(ftime < cfttime);
      assert(ftordered[ftime] == NULL);
      ftordered[ftime] = q;
    }

  // visited = empty_qual_hashset(scratch);
  visited = empty_qual_hashset(NULL);
  qual_hashset scc = empty_qual_hashset(NULL);
  for (i = cfttime - 1; i >= 0; i--)
    {
      assert(ftordered[ftime] != NULL);

      // scc = empty_qual_hashset(scratch);
      qual_hashset_reset(scc);
      reverse_reachable_vars(ftordered[ftime], scc, visited);

      changed |= (qual_hashset_size(scc) > 1);

      scan_qual_hashset(q, qss, scc)
        unify_qual(ftordered[ftime]->u.var.loc,
                   ftordered[ftime]->u.var.loc_index,
                   ftordered[ftime], q, "<compaction>");

      q = ecr_qual(ftordered[ftime]);
    }

  // deleteregion(scratch);
  destroy_qual_hashset(visited);
  destroy_qual_hashset(scc);

  return changed;
}

void graph_size(struct Qual_graph_size *gs)
{
  qual_set_scanner qss;
  qual q;

  unsigned long nodes = 0;
  unsigned long nodes_all = 0;

  unsigned long tlb = 0;
  unsigned long tub = 0;

  unsigned long with_bounds = 0;

  scan_qual_set(q, qss, all_vars)
  {
    nodes_all++;
    if (is_ecr_qual(q))
    {
      nodes++;
      unsigned long nlb = qual_edge_hashset_size(q->u.var.lb);
      unsigned long nub = qual_edge_hashset_size(q->u.var.ub);
      tlb += nlb;
      tub += nub;
      if (q->kind == qk_variable && q->u.var.qbounds)
        with_bounds += 1;
    }
  }
  assert(tlb == tub);
  gs->nodes = nodes;
  gs->nodes_all = nodes_all;
  gs->nodes_with_bounds = with_bounds;
  gs->edges = tlb;
  // TODO: hash table usage
}

// copy OBJ to (a different) region R
#define robjdup(r, obj) memcpy(ralloc(r, sizeof(*obj)), obj, sizeof(*obj))

// TODO: clean_edge_sets() can also be run periodically, cos if not
// USE_OWNING_QUAL_EDGE_HASH_TABLES, it rebuilds hash tables in a new region.

/* Eliminate redundant and self-edges */
static void clean_edge_sets(qual_set vars)
{
  qual q;
  qual_set_scanner qss;

  DEBUG_CHECK_ALL_EDGE_SETS();

#if !USE_QUAL_EDGE_MEMPOOL
  // What I want to do is garbage-collect the region and delete the entire
  // region of old edges; but this doesn't work yet because qual_reason points
  // to qual_edges and I don't have a good way of finding all qual_reasons
  // pointing to a given qual_edge in order to map to the robjdup'ed copy.
  //
  //  Is it worth adding a set of qual_reasons to qual_edge?  I don't think
  //  so...

  // region old_qual_edges_region = qual_edges_region;
  // qual_edges_region = newregion();
#endif

#if !USE_OWNING_QUAL_EDGE_HASH_TABLES
  region old_qual_edge_hashsets_region = qual_edge_hashsets_region;
#if RELEASE_QUAL_EDGE_HASH_TABLE_MEMORY
  qual_edge_hashsets_region = newregion();
#endif
#endif

  // First clear all upper bounds, as we'll just reinsert them from edges by
  // scanning lower bounds.
  scan_qual_set(q, qss, vars)
  {
    if (is_ecr_qual(q)) {
#if USE_OWNING_QUAL_EDGE_HASH_TABLES
      qual_edge_hashset_reset(q->u.var.ub);
#else
      assert(((hash_table)q->u.var.ub)->r == old_qual_edge_hashsets_region);
      q->u.var.ub = empty_qual_edge_hashset(qual_edge_hashsets_region);
#endif
    }
  }

  scan_qual_set(q, qss, vars)
  {
    DEBUG_CHECK_VAR(q);
    if (!is_ecr_qual(q)) continue;

    qual_edge_hashset old_lb = q->u.var.lb;
#if !USE_OWNING_QUAL_EDGE_HASH_TABLES
    assert(((hash_table)old_lb)->r == old_qual_edge_hashsets_region);
#endif
    q->u.var.lb = empty_qual_edge_hashset(qual_edge_hashsets_region);
    qual_edge_hashset_scanner qess; qual_edge e;
    scan_qual_edge_hashset(e, qess, old_lb)
    {
      // I think it's OK, and possibly even necessary in either
      // case, to do this before oink calls updateEcrAll.

      // point edges at ECRs
      e->q1 = ecr_qual(e->q1);
      e->q2 = ecr_qual(e->q2);

      assert(q == e->q2);
      if (!eq_qual(e->q1, e->q2) || (e->kind != ek_flow && !global_qual(q)))
      {
#if !USE_QUAL_EDGE_MEMPOOL
        // e = robjdup(qual_edges_region, e);
#endif
        // quarl 2006-05-13: I think it is OK to not do the hash_search
        // call.  We'll still insert only one of each edge because
        // qual_edge_hashset uses cmp_qual_edge_semantic().  We'll end up
        // using the "last" edge rather than the first, but that shouldn't
        // matter (and could be fixed by introducing a new
        // "insert_noreplace" hash table API call).  Removing these calls
        // give a good speedup (85% of clean_edge_sets()).

        // qual_edge_hashset_hash_search(q->u.var.lb, eq_qual_edge_semantic, e, &e);

        bool inserted1 = qual_edge_hashset_insert(&e->q2->u.var.lb, e);
        bool inserted2 = qual_edge_hashset_insert(&e->q1->u.var.ub, e);
        assert(inserted1 == inserted2);
      }
    }
  }

#if !USE_QUAL_EDGE_MEMPOOL
  // deleteregion(old_qual_edges_region);
#endif

#if !USE_OWNING_QUAL_EDGE_HASH_TABLES && RELEASE_QUAL_EDGE_HASH_TABLE_MEMORY
  deleteregion(old_qual_edge_hashsets_region);
#endif

  DEBUG_CHECK_ALL_EDGE_SETS();
}

bool didCompactifyGraph = FALSE;
/* Simplify the graph to save memory.  This assumes all
   qualifiers attached to externally visible symbols have
   already been flagged as extern, and that the pointers
   from other cqual data structures have been updated to
   point to the ecrs. (see qtype.c:mark_extern_qtype() */
// compactify_graph
void compact_quals_graph(void)
{
  qual_set_scanner qss;
  qual q;
  bool changed = TRUE;

  didCompactifyGraph = TRUE;

  DEBUG_CHECK_ALL_EDGE_SETS();
  // really_check_all_edge_sets();

  graph_size(&qual_stats.pre_compaction);

  // consider unifications at this point to be part of compaction
  pquals_unified = &qual_stats.quals_unified_in_compaction;

  /* Summaries and rk_edge reasons will be destroyed.  Clear them first. */
  scan_qual_set(q, qss, all_vars)
    if (is_ecr_qual(q))
      {
	qual_edge qe;
	qual_edge_hashset_scanner qess;

	scan_qual_edge_hashset(qe, qess, q->u.var.ub)
	  {
	    qe->summary_time = 0;
	    qe->u.error_message = NULL;
	  }

      }

  // after setting summary_time = 0, some edges will now be duplicates;
  // remove them.
  scan_qual_set(q, qss, all_vars)
    if (is_ecr_qual(q))
      {
        qual_edge_hashset_update_hashes(q->u.var.ub);
        qual_edge_hashset_update_hashes(q->u.var.lb);
      }

  DEBUG_CHECK_ALL_EDGE_SETS();
  // DEBUG_CHECK_ALL_QUAL_GATES();

  do
    {
      // DEBUG_CHECK_ALL_QUAL_GATES();
      changed = FALSE;

#if DEBUG_REJECTIONS
      memset(rejections, 0, sizeof(rejections));
#endif

      // DEBUG_CHECK_ALL_QUAL_GATES();

      scan_qual_set(q, qss, all_vars) {
        if (is_ecr_qual(q)) {
          if (check_node_removable(q) != 0)
            continue;
	  changed |= compact_remove_node(q);
          // DEBUG_CHECK_ALL_QUAL_GATES();
        }
      }

      // DEBUG_CHECK_ALL_QUAL_GATES();

      /* Remove disconnected nodes */
      changed |= (qual_set_remove_matches(all_vars, is_dead_node, NULL) > 0);

      // It would be nice to kill the dead_vars entirely; but we can't do that
      // yet because someone might be pointing at them.  (Maybe we can do this
      // in eliminate_quals_links() ?)

      // DEBUG_CHECK_ALL_QUAL_GATES();
      changed |= merge_sccs(all_vars);
      // DEBUG_CHECK_ALL_QUAL_GATES();

      if (changed)
        clean_edge_sets(all_vars);

      // DEBUG_CHECK_ALL_EDGE_SETS();

    } while (changed);

  scan_qual_set(q, qss, all_vars)
    if (is_ecr_qual(q))
      {
	if (q->u.var.qbounds)
	  {
	    qual c;
	    qual_reason qr;
	    qual_reason_map_scanner qrms;

	    scan_qual_reason_map(c, qr, qrms, q->u.var.qbounds->lbr)
	      if (qr->kind == rk_edge)
		{
		  qr->kind = rk_location;
                  qual_edge qe = qr->u.e.qe;
		  qr->u.s.loc = qe->loc;
		  qr->u.s.loc_index = qe->loc_index;
		}

	    scan_qual_reason_map(c, qr, qrms, q->u.var.qbounds->ubr)
	      if (qr->kind == rk_edge)
		{
		  qr->kind = rk_location;
                  qual_edge qe = qr->u.e.qe;
		  qr->u.s.loc = qe->loc;
		  qr->u.s.loc_index = qe->loc_index;
		}
	  }
      }

  pquals_unified = &qual_stats.quals_unified;

  DEBUG_CHECK_ALL_QUAL_GATES();
  DEBUG_CHECK_ALL_EDGE_SETS();
  // really_check_all_edge_sets();

  graph_size(&qual_stats.post_compaction);

#if DEBUG_REJECTIONS
  {
    int i;

    printf("%8s %8s %8s %8s\n",
	   "extern", "size", "edge", "count");
    for (i = 0; i < 16; i++)
      printf("%8s %8s %8s %8s\n",
	     (i & 1) ? "extern"     : " ",
	     (i & 2) ? "size"       : " ",
	     (i & 4) ? "untypable"  : " ",
	     (i & 8) ? "bound-edge" : " ",
	     rejections[i]);

    printf("Graph Size: %d/%d --> %d/%d\n",
	   oldsize, oldedgesize,
	   newsize, newedgesize);

    {
      int lb_edge_set_sizes[32];
      int ub_edge_set_sizes[32];

      memset(lb_edge_set_sizes, 0, sizeof(lb_edge_set_sizes));
      memset(ub_edge_set_sizes, 0, sizeof(ub_edge_set_sizes));

      scan_qual_set(q, qss, all_vars)
	{
	  if (!is_ecr_qual(q))
	    continue;
	  if (qual_edge_hashset_size(q->u.var.lb) >= 32)
	    lb_edge_set_sizes[31]++;
	  else
	    lb_edge_set_sizes[qual_edge_hashset_size(q->u.var.lb)]++;
	  if (qual_edge_hashset_size(q->u.var.ub) >= 32)
	    ub_edge_set_sizes[31]++;
	  else
	    ub_edge_set_sizes[qual_edge_hashset_size(q->u.var.ub)]++;
	}

      printf ("%10s: %10s %10s\n", "size", "lb", "ub");
      for (i = 0; i < 32; i++)
	printf("%10d: %10d %10d\n", i, lb_edge_set_sizes[i], ub_edge_set_sizes[i]);
    }
  }
#endif

}

// TODO: run eliminate_quals_links periodically (e.g. after every 10000
// unifications).  needs to be called from externally (or callback) since we
// have to make sure external qvars are ECRed first.


void make_edges_point_to_ecrs(void)
{
  qual_set_scanner qss;
  qual_edge_hashset_scanner qess;
  qual q;
  qual_edge e;

  DEBUG_CHECK_ALL_EDGE_SETS();

  /* Make edges point to ecrs */
  scan_qual_set(q, qss, all_vars)
    {
      // quarl 2006-05-02
      //    Don't bother with non-ECRs -- their edge hashsets should be
      //    cleared already.
      if (!is_ecr_qual(q)) {
        assert (q->u.var.lb == NULL);
        continue;
      }

      // TODO: isn't e->q2 always q?
      scan_qual_edge_hashset(e, qess, q->u.var.lb)
        {
          e->q1 = ecr_qual(e->q1);
          e->q2 = ecr_qual(e->q2);
        }
    }

  DEBUG_CHECK_ALL_EDGE_SETS();
}

// returns 0 if this qvar is not an ecr representative, i.e. a link node.
// dd_remove_all_matches_from expects 0 to remove.
int qual_ecr_p(qual q, void *dummy)
{
  return (q->link == NULL);
}

static void prune_unused_nodes(void)
{
  // TODO: copy everything we want to keep into a new region and destroy the old
  // one.  There are a significant number of qual and qual_edge we can get rid
  // of.
  //
  // Unfortunately we can't just copy all the quals into a new region and
  // destroy the old one, because oink points at them.  But we can do this for
  // qual_edges.
  //
  // Another possibility is to use mempools so we can "deallocate" them
  // individually.  But if we only ever run compaction at the end of a
  // computation then there's no point deallocating anything.  If we're
  // running out of memory on a single file it might be worth compacting
  // mid-analysis, otherwise linking already-compacted files won't let us
  // compact much mid-analysis.
  //
  // Before doing this we need to run updateEcrAll() in oink.

  /* Now link nodes should all be unused, so remove them */

  qual_set_remove_matches(all_vars, qual_ecr_p, NULL);

//   dd_list_pos scan;
//   dd_list_pos next;

//   for (scan = dd_first(all_vars); !dd_is_end(scan); scan = next)
//   {
//     next = dd_next(scan);
//     qual q = DD_GET(qual, scan);
//     if (!is_ecr_qual(q)) {
//       assert(q->u.var.lb == NULL);
//       assert(q->u.var.ub == NULL);
//       dd_remove(scan);
// #if USE_QUAL_MEMPOOL
//       mempool_free(quals_mempool, q);
//       // TODO: free these? lb_closure, ub_closure, qbounds
// #endif
//     }
//   }

  // qual_set_scanner qss;
  // qual q;
  // qual_hashset qhs;
  // qual_hashset_scanner qhss;
  // region scratch;
  // scratch = newregion();
  // qhs = empty_qual_hashset(scratch);

  // scan_qual_set(q, qss, all_vars)
  //   qual_hashset_insert(&qhs, ecr_qual(q));

  // all_vars = empty_qual_set(quals_region);

  // scan_qual_hashset(q, qhss, qhs)
  //   qual_set_insert_nocheck(quals_region, &all_vars, q);

  // deleteregion(scratch);

}

/* Eliminate all link nodes to save memory. Assumes all external
   pointers into the quals graph have already been updated to point
   to ecr's. */
void eliminate_quals_links(void)
{
  make_edges_point_to_ecrs();
  prune_unused_nodes();
}


/**************************************************************************
 *                                                                        *
 * State transitions and results generation                               *
 *                                                                        *
 **************************************************************************/


static void do_print_quals_graph(qual_set important_vars)
{
  char* filename = NULL;
  char* rootname = NULL;
  int depth;
  FILE* f = NULL;
  qual root = NULL;

  /* Decide on a filename */
  filename = getenv ("CQUAL_GRAPH_FILE");
  if (filename == NULL)
    filename = "quals.dot";

  /* See if we should only print out some nbd. */
  rootname = getenv ("CQUAL_GRAPH_ROOT");
  if (rootname)
    {
#if DELAY_SET_NAMES
      fprintf(stderr, "ERROR: CQUAL_GRAPH_ROOT not yet supported when DELAY_SET_NAMES is on\n");
#else
      root = find_var (rootname);
      if (root)
	printf ("found root\n");
      else
	printf ("couldn't find root\n");
#endif
    }

  /* If we are only doing a nbd, figure out how big the nbd should
     be. */
  if (rootname)
    {
      if (getenv ("CQUAL_GRAPH_DEPTH"))
	depth = atoi (getenv ("CQUAL_GRAPH_DEPTH"));
      else
	depth = 3;
    }
  else
    depth = INT_MAX;

  /* Do it. */
  f = fopen (filename, "w");
  if (f)
    {
      if (root)
	dump_neighborhood (f, all_vars, root, depth);
      else
	dump_quals_graph (f, important_vars);
      fclose (f);
    }
}

/**************************************************************************
 *                                                                        *
 * State transitions                                                      *
 *                                                                        *
 **************************************************************************/

void init_quals()
{
  DEBUG_INIT();

  assert(!quals_permanent_region);

  memset(&qual_stats, 0, sizeof(qual_stats));
  pquals_unified = &qual_stats.quals_unified;


  quals_permanent_region = newregion();
  quals_region = newregion();
#if USE_QUAL_EDGE_MEMPOOL
  mempool_init(qual_edges_mempool, sizeof(struct Qual_edge));
#else
  qual_edges_region = newregion();
#endif
#if ! USE_OWNING_QUAL_EDGE_HASH_TABLES
  qual_edge_hashsets_region = newregion();
#endif
  resolve_region = newregion();
  quals_s18n_region = quals_permanent_region;

  next_qual = 0;
  all_quals = empty_qual_set(quals_permanent_region);
  all_vars = empty_qual_set(quals_permanent_region);
  dead_vars = empty_qual_set(quals_permanent_region);
  all_pos = empty_po_set(quals_permanent_region);
  current_po = NULL;
  qinstantiations = new_qinstantiation_list(resolve_region);

  qual_gate_init (&empty_qgate, TRUE, TRUE);

  /* Quals will be added to these gates as they are created */
  qual_gate_init (&open_qgate, TRUE, TRUE);
  qual_gate_init (&fi_qgate, TRUE, TRUE);
  qual_gate_init (&fs_qgate, TRUE, TRUE);
  qual_gate_init (&effect_qgate, TRUE, TRUE);
  qual_gate_init (&casts_preserve_qgate, TRUE, TRUE);

  /* The qgates are unidirectional because of sign_eq quals. */
  qual_gate_init (&ptrflow_down_pos_qgate, FALSE, TRUE);
  qual_gate_init (&ptrflow_up_pos_qgate, FALSE, TRUE);
  qual_gate_init (&ptrflow_down_neg_qgate, TRUE, FALSE);
  qual_gate_init (&ptrflow_up_neg_qgate, TRUE, FALSE);

  qual_gate_init (&fieldflow_down_pos_qgate, FALSE, TRUE);
  qual_gate_init (&fieldflow_up_pos_qgate, FALSE, TRUE);
  qual_gate_init (&fieldflow_down_neg_qgate, TRUE, FALSE);
  qual_gate_init (&fieldflow_up_neg_qgate, TRUE, FALSE);

  qual_gate_init (&fieldptrflow_down_pos_qgate, FALSE, TRUE);
  qual_gate_init (&fieldptrflow_up_pos_qgate, FALSE, TRUE);
  qual_gate_init (&fieldptrflow_down_neg_qgate, TRUE, FALSE);
  qual_gate_init (&fieldptrflow_up_neg_qgate, TRUE, FALSE);

  assert(num_hotspots >= 0);
  if (num_hotspots != 0)
    /* rarrayalloc initializes hotspots to 0 */
    {
      int i;

      hotspots = rarrayalloc(quals_permanent_region, num_hotspots, qual);
      for (i = 0; i < num_hotspots; i++)
	hotspots[i] = NULL;
    }
  else
    hotspots = NULL;

  state = state_init;
}

void end_define_pos(void)
{
  assert(state == state_init);
  state = state_pos_defined;
}

int finish_quals(void)
{
  DEBUG_CHECK_ALL_EDGE_SETS();

  graph_size(&qual_stats.pre_finish);

  /* The set of vars which are reachable (not necessarily CFL reachable)
     from the constants. */
  qual_set important_vars;

  /* dsw: I removed resolve_wellformed() and _resolve_wellformed()
     because 1) they were unused and 2) they depended on termhash.
     Rob said we should get rid of termhash and the
     resolve_wellformed() functions were the only reason we couldn't */
  /* resolve_wellformed(); */

  if (need_cfl_reset)
    reset_cfl_flags(all_vars);

  /* Cull un-interesting variables */
  if (flag_errors_only)
    important_vars = find_important_vars(all_vars);
  else
    important_vars = all_vars;

  /* CHECK_ALL_VARS(); */

  if (flag_print_quals_graph)
    do_print_quals_graph(important_vars);

  cfl_flow (important_vars);

  state = state_finish;

  int ret = report_errors(all_vars);

  graph_size(&qual_stats.post_finish);

  return ret;
}

void reset_quals_edges(void)
{
#if USE_OWNING_QUAL_EDGE_HASH_TABLES && RELEASE_QUAL_EDGE_HASH_TABLE_MEMORY
  qual_set_scanner qss;
  qual q;

  // fprintf(stderr, "## reset_quals_edges(): all_vars=%p\n", all_vars);

  scan_qual_set(q, qss, all_vars) {
    if (is_ecr_qual(q)) {
      // fprintf(stderr, "## reset_quals_edges() all_vars: destroying %p\n", q);
      assert(q->kind == qk_variable);
      assert(q->u.var.lb);
      assert(q->u.var.ub);

      destroy_qual_edge_hashset(q->u.var.lb);
      destroy_qual_edge_hashset(q->u.var.ub);
    } else {
      // fprintf(stderr, "## reset_quals_edges() all_vars: not ecr: %p\n", q);
      assert(q->u.var.lb == NULL);
      assert(q->u.var.ub == NULL);
    }
  }

  scan_qual_set(q, qss, dead_vars) {
    if (is_ecr_qual(q)) {
      // fprintf(stderr, "## reset_quals_edges() dead_vars: destroying %p\n", q);
      assert(q->kind == qk_variable);
      assert(q->u.var.lb);
      assert(q->u.var.ub);

      destroy_qual_edge_hashset(q->u.var.lb);
      destroy_qual_edge_hashset(q->u.var.ub);
    } else {
      // fprintf(stderr, "## reset_quals_edges() dead_vars: not ecr: %p\n", q);
      assert(q->u.var.lb == NULL);
      assert(q->u.var.ub == NULL);
    }
  }

  // scan_qual_set(q, qss, all_quals) {
  // }
#else
  // nothing to do - we destroy the region in reset_quals().
#endif
}

/* Dipose of all memory and reset to initial state.  Don't call this
   function; instead, call reset_libqual() */
void reset_quals(void) {
  reset_quals_edges();

  state = state_start;
  exists_effect_qual = FALSE;
  exists_casts_preserve_qual = FALSE;
  exists_ptrflow_qual = FALSE;
  exists_fieldflow_qual = FALSE;
  exists_fieldptrflow_qual = FALSE;
  used_fs_qual = FALSE;
  flag_print_quals_graph = 0;
  flag_poly = 0;
  flag_errors_only = 0;
  flag_context_summary = 0;
  flag_explain_errors = 1;
  next_qual = 0;
  all_quals = NULL;
  all_vars = NULL;
  dead_vars = NULL;
  all_pos = NULL;
  current_po = NULL;
  num_constants = 0;
  current_summary_time = 1;
  memset(&empty_qgate, 0, sizeof(qual_gate));
  memset(&open_qgate, 0, sizeof(qual_gate));
  memset(&fi_qgate, 0, sizeof(qual_gate));
  memset(&fs_qgate, 0, sizeof(qual_gate));
  memset(&effect_qgate, 0, sizeof(qual_gate));
  memset(&casts_preserve_qgate, 0, sizeof(qual_gate));
  memset(&ptrflow_down_pos_qgate, 0, sizeof(qual_gate));
  memset(&ptrflow_down_neg_qgate, 0, sizeof(qual_gate));
  memset(&ptrflow_up_pos_qgate, 0, sizeof(qual_gate));
  memset(&ptrflow_up_neg_qgate, 0, sizeof(qual_gate));
  memset(&fieldflow_down_pos_qgate, 0, sizeof(qual_gate));
  memset(&fieldflow_down_neg_qgate, 0, sizeof(qual_gate));
  memset(&fieldflow_up_pos_qgate, 0, sizeof(qual_gate));
  memset(&fieldflow_up_neg_qgate, 0, sizeof(qual_gate));
  memset(&fieldptrflow_down_pos_qgate, 0, sizeof(qual_gate));
  memset(&fieldptrflow_down_neg_qgate, 0, sizeof(qual_gate));
  memset(&fieldptrflow_up_pos_qgate, 0, sizeof(qual_gate));
  memset(&fieldptrflow_up_neg_qgate, 0, sizeof(qual_gate));
  const_qual = NULL;
  nonconst_qual = NULL;
  volatile_qual = NULL;
  restrict_qual = NULL;
  noreturn_qual = NULL;
  init_qual = NULL;
  noninit_qual = NULL;
  num_hotspots = 0;
  hotspots = NULL;
  qinstantiations = NULL;
  print_graph_file_target = NULL;
  /* Plus a few statics! */
  if (quals_permanent_region) {
    deleteregion(quals_permanent_region);
    quals_permanent_region = NULL;
  }
  if (quals_region) {
    deleteregion(quals_region);
    quals_region = NULL;
  }
#if USE_QUAL_EDGE_MEMPOOL
  // nothing to do - don't need to (and can't) destroy qual_edges_mempool
#else
  if (qual_edges_region) {
    deleteregion(qual_edges_region);
    qual_edges_region = NULL;
  }
#endif

#if USE_OWNING_QUAL_EDGE_HASH_TABLES
  // nothing to do
  assert (qual_edge_hashsets_region == NULL);
#else
  if (qual_edge_hashsets_region) {
    deleteregion(qual_edge_hashsets_region);
    qual_edge_hashsets_region = NULL;
  }
#endif

  if (resolve_region) {
    deleteregion(resolve_region);
    resolve_region = NULL;
  }
#if DEBUG_EXTERN_QUAL
  if (extern_quals_region) {
    deleteregion(extern_quals_region);
    extern_quals_region = NULL;
  }
#endif

}

static inline
void printGraphSize(char const *n, struct Qual_graph_size const *gs)
{
  printf("%s qual nodes: %lu ECR (%lu w/bounds, %lu total); edges: %lu\n",
         n, gs->nodes, gs->nodes_with_bounds, gs->nodes_all, gs->edges);
}

void Qual_print_stats(void)
{
  graph_size(&qual_stats.final);

  printf("quals created: %lu\n", qual_stats.quals_created);
  printf("      unified: %lu\n", qual_stats.quals_unified);
  printf("      unified in compaction: %lu\n", qual_stats.quals_unified_in_compaction);
  printf("      removed in compaction: %lu\n", qual_stats.quals_removed_in_compaction);
  printf("      killed: %lu\n", qual_stats.quals_killed);
  printf("qual_edges created: %lu\n", qual_stats.qual_edges_created);
  printf("      summary edges created: %lu\n", qual_stats.summary_edges_created);

  printf("          max depths:\n");
  printf("              find_bak_summary: %lu, _find_bak_summary: %lu\n",
         qual_stats.find_bak_summary_depth_max,
         qual_stats._find_bak_summary_depth_max);
  printf("              find_fwd_summary: %lu, _find_fwd_summary: %lu\n",
         qual_stats.find_fwd_summary_depth_max,
         qual_stats._find_fwd_summary_depth_max);

  assert(qual_stats.find_bak_summary_depth == 0);
  assert(qual_stats.find_fwd_summary_depth == 0);
  assert(qual_stats._find_bak_summary_depth == 0);
  assert(qual_stats._find_fwd_summary_depth == 0);

  printGraphSize("Pre-finish", &qual_stats.pre_finish);
  printGraphSize("Post-finish", &qual_stats.post_finish);
  if (didCompactifyGraph) {
    printGraphSize("Pre-compaction", &qual_stats.pre_compaction);
    printGraphSize("Post-compaction", &qual_stats.post_compaction);
  }
  printGraphSize("Final", &qual_stats.final);

#define T(N) printf(#N "s created: %lu\n", num_##N##s_created)
  T(qual_hashset);
  T(qual_edge_hashset);
  T(qual_reason_map);
  T(qual_edge_pred_map);
  T(qual_gate_visit_map);
  T(qual_intptr_t_map);
#undef T

  // locations_print_stats();
  regions_print_stats();

#ifdef DEBUG_HASHSET_NEXT
  extern void hashset_table_next_debug_report();
  hashset_table_next_debug_report();
#endif
}

void Qual_print_sizes(void)
{
#define P(T) printf("  sizeof(" #T ") = %d\n", (int) sizeof(struct T))
  printf("LibQual:\n");
  P(Type_qualifier);
  printf("    u.var: %d\n", (int) sizeof( ((qual)0)->u.var ));
  printf("    u.elt: %d\n", (int) sizeof( ((qual)0)->u.elt ));
  P(Qual_edge);
  P(Qual_gate);
  P(Qual_reason);
  P(Qual_bounds);
  printf("\n");
#if defined HASH_EMBED_INLINE
  P(Hash_table);
  P(Hashset_table);
#endif
#undef P
}

static void DEBUG_really_init(void)
{
  // printf("sizeof qual_gate = %d\n", sizeof(qual_gate));
  // printf("sizeof struct Qual_edge = %d\n", sizeof(struct Qual_edge));
  // abort();

  // qual_gate qgate;
  // memset(&qgate, 0, sizeof(qgate));
  // qual_gate_set_backward(&qgate, 1);
  // qual_gate_set_forward(&qgate, 1);

  // printf("m = %x\n", qgate.mask[0] == GATE_FLAGWORD_FLAG_MASK);
}

// TODO: if we embed 'next' pointers into 'qual' and get rid of all_vars, we
// can save some bytes, and also remove them from the list without searching

// TODO: when generating dot output, color the edges representing error paths
