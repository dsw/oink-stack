/* see License.txt for copyright and terms of use */

#ifndef QUALS_H
#define QUALS_H

#include "libqual/libqual.h"
#include "libqual/typed_set.h"
#include "libqual/typed_bag.h"
#include "libqual/location.h"
#include "libqual/serialize.h"
#include "libregion/regions.h"

// quarl 2006-06-23
//    To save time and especially memory, we now do not assign names until we
//    are about to print the error messages, at which point we know for which
//    qvars we need names.
#define DELAY_SET_NAMES 1

/**************************************************************************
 *                                                                        *
 * Types and globals                                                      *
 *                                                                        *
 **************************************************************************/

/* A type qualifier */
typedef struct Type_qualifier *qual;
extern s18n_type qual_type;
extern s18n_type qual_set_type;

/* A set of type qualifiers */
int cmp_qual_noninline(qual left, qual right);
int cmp_qual_byname(qual left, qual right);
DEFINE_SET(qual_set,qual,cmp_qual_noninline);
#define scan_qual_set(var, scanner, set) \
for (qual_set_scan(set, &scanner), var = qual_set_next(&scanner); \
     var; \
     var = qual_set_next(&scanner))


/* Edges in the constraint graph */
typedef struct Qual_edge* qual_edge;

/* A set of edges */
DEFINE_BAG(qual_edge_set, qual_edge);
#define scan_qual_edge_set(var,scanner,set) \
for (qual_edge_set_scan(set, &scanner), var = qual_edge_set_next(&scanner); \
     var; \
     var = qual_edge_set_next(&scanner))


/* True if we should print out the qualifier constraint graph in dot
   form. */
extern int flag_print_quals_graph;

/* True if polymorphism is enabled */
extern int flag_poly;

typedef enum { level_value, level_ref } level_qual_t;
typedef enum { sign_pos, sign_neg, sign_eq } sign_qual_t;
typedef enum { flow_up, flow_down } flow_qual_t;


// quarl 2006-05-24
//   MAX_QUALS defines how many gates are supported.  It must be a multiple of
//   BITS_PER_GATE_WORD (32 on 32-bit machines), or 2 less than such.
//   The whole structure takes ceil((MAX_QUALS+2)/8) bytes.
//
//   By using e.g. 30 bytes, we pack the two flags ('forward' and 'backward')
//   and the gates in the same word, to save space.
//
//   Be careful what you set MAX_QUALS to.  Qual_edge, which contains
//   Qual_gate, is the most memory-intensive structure in cqual.
//
//   If the whole structure fits in one word, it can be passed much more
//   efficiently (without pointers).

// #define MAX_QUALS 64
// #define MAX_QUALS 32
#define MAX_QUALS 30

typedef unsigned int gate_word;
// BITS_PER_GATE_WORD is 32 on 32-bit machines, etc.
#define BITS_PER_GATE_WORD (sizeof (gate_word) * 8)

#if (MAX_QUALS & 31) == 0
#define GATE_WORDS (((MAX_QUALS) + BITS_PER_GATE_WORD - 1) / BITS_PER_GATE_WORD)
#define GATE_WORDS_FULL (GATE_WORDS)
#define GATE_WORDS_PACK_FLAGS 0

#elif ((MAX_QUALS+2) & 31) == 0
#define GATE_WORDS (((MAX_QUALS) + BITS_PER_GATE_WORD - 1) / BITS_PER_GATE_WORD)
#define GATE_WORDS_FULL (GATE_WORDS-1)
#define GATE_FLAGWORD_INDEX (GATE_WORDS-1)
#define GATE_FLAGWORD_FLAG_MASK (3 << (BITS_PER_GATE_WORD-2))
#define GATE_FLAGWORD_NOFLAG_MASK (~ GATE_FLAGWORD_FLAG_MASK)
#define GATE_WORDS_PACK_FLAGS 1
#define GATE_BIT_FORWARD (BITS_PER_GATE_WORD-1)
#define GATE_BIT_BACKWARD (BITS_PER_GATE_WORD-2)

#else
#error "Invalid MAX_QUALS"
#endif


typedef struct Qual_gate {
  gate_word mask[GATE_WORDS];
#if GATE_WORDS_PACK_FLAGS
  // forward === mask[GATE_FLAGWORD_INDEX][GATE_BIT_FORWARD]
  // backward === mask[GATE_FLAGWORD_INDEX][GATE_BIT_BACKWARD]
#else
  // Put the non-word-aligned members at the end in case of packing.
  bool forward:1;
  bool backward:1;
#endif
} /*__attribute__((packed)) */ qual_gate;

/* C qualifiers */
extern qual const_qual;
extern qual nonconst_qual;
extern qual volatile_qual;
extern qual restrict_qual;
extern qual noreturn_qual; /* __attribute__((noreturn)) */
extern qual init_qual;     /* __init */
extern qual noninit_qual;

/* exists_X_qual is set to TRUE if any X qualifiers have been defined in any
   partial orders */
extern bool exists_effect_qual;
extern bool exists_casts_preserve_qual;
extern bool exists_ptrflow_qual;
extern bool exists_fieldflow_qual;
extern bool exists_fieldptrflow_qual;

/* used_X_qual is set to TRUE if after end_define_pos() there have been any
   calls to find_qual to look up an X qualifier */
extern bool used_fs_qual;

/* Common edge gates */
extern qual_gate open_qgate;               /* all quals */
extern qual_gate fi_qgate;                 /* flow-insensitive quals */
extern qual_gate fs_qgate;                 /* flow-sensitive quals */
extern qual_gate effect_qgate;             /* effect quals */
extern qual_gate casts_preserve_qgate;     /* for collapsing below pathological casts */

extern qual_gate ptrflow_down_pos_qgate;
extern qual_gate ptrflow_down_neg_qgate;
extern qual_gate ptrflow_up_pos_qgate;
extern qual_gate ptrflow_up_neg_qgate;

extern qual_gate fieldflow_down_pos_qgate;
extern qual_gate fieldflow_down_neg_qgate;
extern qual_gate fieldflow_up_pos_qgate;
extern qual_gate fieldflow_up_neg_qgate;

extern qual_gate fieldptrflow_down_pos_qgate;
extern qual_gate fieldptrflow_down_neg_qgate;
extern qual_gate fieldptrflow_up_pos_qgate;
extern qual_gate fieldptrflow_up_neg_qgate;

typedef void (*qual_traverse_fn)(qual q, void *arg);

typedef void (*edge_traverse_fn)(qual left, qual right, bool bidi,
                                 location loc, int loc_index,
				 polarity p, bool first, void *arg);

/* Human-readable meaning of p */
const char *polarity_to_string(polarity p);

/**************************************************************************
 *                                                                        *
 * State transitions                                                      *
 *                                                                        *
 * These functions are in general called in the order listed in this      *
 * file.                                                                  *
 *                                                                        *
 **************************************************************************/

void init_quals(void);        /* Called before any other functions in
				 quals.h are called. */
void end_define_pos(void);    /* Called when no more partial orders will
				 be defined.  You *must* call this function
			         before creating any qualifier variables. */
int finish_quals(void);       /* Called when no more constraints will
				 be generated. Returns 0 for no qualifier
				 errors, 1 if there were qualifier errors */
void reset_quals(void);       /* Dipose of all memory and reset to
				 initial state.  Don't call this
				 function; instead, call reset_libqual() */

/**************************************************************************
 *                                                                        *
 * Partial order elements                                                 *
 *                                                                        *
 **************************************************************************/

/* Begin and end defining a partial order.  All the qualifiers created
   between calls to begin and end are exclusively related. */
void begin_po_qual(void);
void end_po_qual(void);

void set_po_flow_sensitive(void);         /* Marks the current partial order
					     as flow-sensitive */
void set_po_nonprop(void);                /* Marks the current partial order
					     as non-propagating */
void set_po_effect(void);		  /* Marks the current partial order
					     as able to propagate over
					     effect edges */
void set_po_casts_preserve(void);	  /* Marks the current partial order
					     as able to propagate over
					     casts_preserve edges */

qual add_qual(const char *name);          /* Make a new (constant) qualifier */
void add_qual_lt(qual left, qual right);  /* Add assumption left < right */
void add_color_qual(qual q, const char *color); /* Add color for qualifier */
void add_level_qual(qual q, level_qual_t lev);
                                          /* Assert that q qualifies lev */
void add_sign_qual(qual q, sign_qual_t sign);
                                          /* Assert that q has variance sign */
void add_ptrflow_qual(qual q, flow_qual_t f);
                                          /* Assert that q flows up or down
                                             the ptr type hierarchy. */
void add_fieldflow_qual(qual q, flow_qual_t f);
                                          /* Assert that q flows up or down
                                             the field type hierarchy. */

void add_fieldptrflow_qual(qual q, flow_qual_t f);
                                          /* Assert that q flows up or down
                                             the fieldptr type hierarchy. */

qual_set get_constant_quals (void);      /* Return the set of all constants */
qual find_qual(const char *name);        /* Look up a qualifier */
level_qual_t level_qual(qual q);         /* Return the level of qualifier q */
sign_qual_t sign_qual(qual q);           /* Return the sign of qualifier q */
bool flow_sensitive_qual(qual q);        /* Return TRUE iff q is flow-
					    sensitive */
bool nonprop_qual(qual q);               /* Return TRUE iff q is
					    non-propagating */
bool variable_qual(qual q);              /* Return TRUE iff q is a variable */
bool constant_qual(qual q);              /* Return TRUE iff q is a constant */

// bool qual_gate_set_qual (qual_gate *qgate, qual q, bool allow);

/**************************************************************************
 *                                                                        *
 * Qualifier variables                                                    *
 *                                                                        *
 **************************************************************************/

/* Get the canonical representative of a qualifier */
qual ecr_qual_noninline(qual q);

/* Make a new, distinct qualifier variable:
 *
 *    - name: a user-readable name for this qual.
 *
 *    - loc: the program location for this qual.
 *
 *    - preferred: whether this qual should be preferred as an equivalence
 *                 class representative.
 *
 *    - global: whether this qual decorates a global variable.
 */
qual make_qvar(
#if !DELAY_SET_NAMES
  const char *name,
#endif
  location loc, int loc_index,
  bool preferred, bool global);

#if DELAY_SET_NAMES
// quarl 2006-06-26
//
// These must be provided by the client; the API is:
//   finish_qvar():
//     1. calls declare_qvar_need_name() on qvars that need names.
//     2. calls assign_qvar_names().
//     3. calls qvar_get_name() on qvars that need names.
//
// The idea is that few qvars need names, and assigning names is expensive (in
// time and memory), so don't do it except for those qvars that need names.
// Thus we also don't need the pointer to names and also don't need to
// serialize names.  (This used to take O(n*m) space where N is the size of
// names in the program and M is the depth of the tree, and 2x that if
// deserializing.)
//
// These only operate on ECRs.
extern void declare_qvar_need_name(qual qvar);
extern void assign_qvar_names(void);
extern char const *qvar_get_name(qual qvar);
#endif

/* Make a fresh qualifier variable with a unique name
 *
 *    - base_name: a user-readable basename for this qual.
 *
 *    - loc: the program location for this qual.
 */
qual make_fresh_qvar(const char *base_name, location loc, int loc_index);

/* Get the set of all variables */
qual_set get_variable_quals (void);

/* Add a closure c that's invoked whenever a lower bound is added to q */
void add_lb_trigger_qual(qual q, closure c);

/* Add a closure c that's invoked whenever an upper bound is added to q */
void add_ub_trigger_qual(qual q, closure c);

/* Check if qualifier flows into this qual at cast */
void mk_no_qual_qual(location loc, int loc_index, qual q);

void set_global_qual (qual q, bool global); /* Change the globality of a qualifier */
void set_extern_qual (qual q, bool is_extern); /* Set the externality of a qualifier */
void set_extern_all_qual (bool is_extern); /* Set all qvars to given externality */
bool extern_qual (qual q); /* TRUE iff q is extern */

void set_defined_qual (qual q, bool defined); /* Set whether a qualifier on a function type
						 is fully defined */

// bool global_qual(qual q);            /* Return TRUE if q qualifies a global */
bool is_dead_qual(qual q);           // Return TRUE iff q was killed in compaction
bool preferred_qual(qual q);         /* Return TRUE iff q is preferred */
// location location_qual(qual q);      /* Where q came from */
qual_edge_set lb_qual(region r, qual q);       /* Lower-bounds (generated) of q */
qual_edge_set cs_lb_qual(region r, qual q);    /* Context-summary lower-bounds
						  (generated) of q */
qual_edge_set ub_qual(region r, qual q);       /* Upper-bounds (generated) of q */
qual_edge_set cs_ub_qual(region r, qual q);    /* Context-summary upper-bounds
						  (generated) of q */
int error_count_qual(qual q);        /* Number of error paths q is
					involved in */

/* Whether a qualifier is an effect qualifier */
bool effect_qual(qual q);

/* Apply f to each possible value of q */
void scan_qual_bounds(qual q, qual_gate *qgate, qual_traverse_fn f, void *arg);

/* Apply f to each possible value of q in sorted order */
void scan_qual_bounds_sorted(qual q, qual_gate *qgate, qual_traverse_fn f, void *arg);

/**************************************************************************
 *                                                                        *
 * Qualifier graph edges                                                  *
 *                                                                        *
 **************************************************************************/

/* Given a qualifier edge qe of the form q_0 -> q_1, if q == q_i, then
   this returns q_{1-i}. */
qual qual_edge_other_end (qual_edge qe, qual q);

/* Returns the location in the program that caused qe to be created. */
// location qual_edge_loc (qual_edge qe);

/* Set whether qualifiers in the same po as q are allowed throug this gate. */
void qual_gate_set_qual_po (qual_gate *qgate, qual q, bool allow);

/**************************************************************************
 *                                                                        *
 * Generate/solve constraints                                             *
 *                                                                        *
 **************************************************************************/

/* Create a flow edge w.r.t. qualifiers in qgate from left to right */
void mkleq_qual_en(location loc, int loc_index,
                   qual_gate *qgate, qual left, qual right,
                   const char *error_message,
                   int edgeNumber  /* unique id for the edge */
                   );

/* Make left <= right and vice-versa, w.r.t. qualifiers in qgate */
void mkeq_qual_en(location loc, int loc_index,
                  qual_gate *qgate, qual left, qual right,
                  const char *error_message,
                  int edgeNumber   /* unique id for the edge */
                  );

/* Create a call edge (p = p_neg), return edge (p = p_pos), or
   call-return edges (p = p_non) from left to right. */
void mkinst_qual_en(location loc, int loc_index,
                    qual_gate *qgate, qual left, qual right, polarity p,
                    const char *error_message,
                    int edgeNumber /* unique id for the edge */
                    );

/* Make left and right indistinguishable. */
void unify_qual_en(location loc, int loc_index,
                   qual left, qual right, const char * error_message,
                   int edgeNumber);

/* Create a flow edge w.r.t. qualifiers in qgate from left to right */
void mkleq_qual(location loc, int loc_index,
                qual_gate *qgate, qual left, qual right,
		const char *error_message);

/* Make left <= right and vice-versa, w.r.t. qualifiers in qgate */
void mkeq_qual(location loc, int loc_index,
               qual_gate *qgate, qual left, qual right,
	       const char *error_message);

/* Create a call edge (p = p_neg), return edge (p = p_pos), or
   call-return edges (p = p_non) from left to right. */
void mkinst_qual(location loc, int loc_index,
                 qual_gate *qgate, qual left, qual right, polarity p,
		 const char *error_message);

/* Make left and right indistinguishable. */
void unify_qual(location loc, int loc_index,
                qual left, qual right, const char *error_message);

/* Adds the conditional constraint l1<=r1 ==> l2<=r2.  This constraint
   is allowed only if at least one of {l1,r1} is a constant. */
void cond_mkleq_qual(location loc, int loc_index,
                     qual_gate *qgate1, qual l1, qual r1,
		     qual_gate *qgate2, qual l2, qual r2,
		     const char * error_message);

/* Copy the constraint described by e to the identical constraint
   between q1 and q2. */
void copy_constraint (qual_edge e, qual q1, qual q2);

/**************************************************************************
 *                                                                        *
 * Queries (no generated constraints)                                     *
 *                                                                        *
 **************************************************************************/

//  bool eq_qual(qual left, qual right);     /* Returns TRUE iff left and
//  					    right are the same.  Does
//  					    not generate a constraint. */
bool leq_qual(qual left, qual right);    /* Return TRUE iff the
                                            constraints entail q1 <=
                                            q2, where at least one of
                                            q1 and q2 is a
                                            constant. Returns FALSE if
                                            q1 and q2 are constants in
                                            distinct partial orders.
                                            This function can only be
                                            called after
                                            finish_quals() */
bool cs_leq_qual(qual left, qual right); /* Return TRUE iff the
                                            constraints entail q1 <=
                                            q2 in the context summary,
                                            where at least one of q1
                                            and q2 is a
                                            constant. Returns FALSE if
                                            q1 and q2 are constants in
                                            distinct partial orders.
                                            This function can only be
                                            called after
                                            finish_quals() */

/* Return TRUE iff q has bound as an originally-specified lower-bound.
   If bound == NULL, return whether q has any original lower-bound. */
bool has_lb_qual(qual q, qual bound);

/* Return TRUE iff q has bound as an originally-specified lower-bound
   in the context summary. */
bool cs_has_lb_qual(qual q, qual bound);

/* Return TRUE iff q has bound as an originally-specified upper-bound .
   If bound == NULL, return whether q has any original upper-bound. */
bool has_ub_qual(qual q, qual bound);

/* Return TRUE iff q has bound as an originally-specified upper-bound
   in the context summary. */
bool cs_has_ub_qual(qual q, qual bound);

/* Return TRUE iff q has bound as an originally-specified bound */
bool has_qual(qual q, qual bound);

/* Return TRUE iff q has bound as an originally-specified bound in the
   context summary. */
bool cs_has_qual(qual q, qual bound);

/* Return TRUE iff q has any flow-sensitive qualifier as an
   originally-specified bound. */
bool has_fs_qual(qual q);

/* Return TRUE iff the constraint q1 <= q2 is consistent with the
   previously specified constraints.  Returns TRUE if q1 and q2 are
   constants in distinct partial orders.  This function can only be
   called after finish_quals() */
bool may_leq_qual (qual q1, qual q2);

/* Return TRUE iff the constraint q1 <= q2 is consistent with the
   previously specified constraints (using context-summary).  Returns
   TRUE if q1 and q2 are constants in distinct partial orders.  This
   function can only be called after finish_quals() */
bool cs_may_leq_qual (qual q1, qual q2);

typedef int (*pr_qual_fn)(printf_func, qual, qual_gate *);
int print_qual(printf_func, qual, qual_gate *qgate);    /* Print a qualifier, nice. */
int cs_print_qual(printf_func, qual, qual_gate *qgate); /* Print context-summary of a qualifier, nice. */
int print_qual_raw(printf_func, qual, qual_gate *qgate);   /* Print a qualifier, ugly */
            /* Both functions return the number of characters printed. */

/* Return the name of qualifier q.  q may be a constant or a
variable. */
const char *name_qual(qual q);


#if !DELAY_SET_NAMES
/* Set a qualifier's name. Returns TRUE if the name was successfully set */
bool set_qual_name(qual q, const char *name);
#endif

/* Return the unique internal name used to identify q (q's address) */
const char *unique_name_qual(qual q);

/* Return the color of qualifier q.  q may be a constant or a
variable. */
const char *color_qual(qual q, qual_gate *qgate);

/* Return TRUE iff this qualifier represents an error.  Will always
return FALSE for a constant. */
bool error_qual(qual q, qual_gate *qgate);

/* Return the constant bound on q that should determine
   the color of q, or NULL if no bounds on q, or
   -1 if q has conflicting bounds. */
qual color_bound_qual(qual q, qual_gate *qgate);
/* Given the color bounds q1 and q2, on different levels of a qtype,
   choose the color bound for the whole qtype. */
qual color_bound_qual_qtype_combine(qual q1, qual q2);

/* Traverse the qualifier graph */

/* Print the graph of qualifiers for q to file.  If po_bounds_only is
   true, then only print the graph showing how partial order elements
   affect q; otherwise print all qualifiers reachable from q. */
void print_qual_graph(qual q, const char *file, bool po_bounds_only);

/* Call f(q1, q2, arg) for every edge q1->q2 on a shortest
   unidirectional path from qualifier variable q to constant qualifier
   c.  If c == NULL, finds shortest path to any constant qualifier.
   If there are paths in both directions, traverses both. */
void traverse_shortest_path_edges(qual q, qual c, edge_traverse_fn f,
				  void *arg);

void traverse_error_path_edges(qual q,bool expand_summaries, bool nonerrors,
			       edge_traverse_fn f,void* arg);
void cs_traverse_error_path_edges(qual q,bool expand_summaries, bool nonerrors,
				  edge_traverse_fn f,void* arg);

extern int num_hotspots;   /* Number of hotspots to track */
extern qual *hotspots;     /* Array of num_hotspots qualifiers
                              involved in error paths */

/**************************************************************************
 *                                                                        *
 * serialization                                                          *
 *                                                                        *
 **************************************************************************/

/* Remove extra garbage before serializing */
void compact_quals_graph(void);
void eliminate_quals_links(void);

/* Serialize the quals graph */
int serialize_quals_graph(s18n_context *sc);
void merge_quals_graph(s18n_context *sc);

/* Hacks */

/* Change flow-sensitive quals from nonprop to regular quals for this pass */
void reset_flow_sensitive_quals(void);

/**************************************************************************
 *                                                                        *
 * debugging                                                              *
 *                                                                        *
 **************************************************************************/

void check_all_edge_sets(void);

// statistics

struct Qual_graph_size {
  unsigned long nodes;                 // number of ECR quals
  unsigned long nodes_all;             // number of ECR/non-ECR quals
  unsigned long nodes_with_bounds;     // number of ECR quals with bounds
  unsigned long edges;                 // number of qual edges
};

struct Qual_stats {
  unsigned long quals_created;
  unsigned long quals_unified;
  unsigned long quals_removed_in_compaction;
  unsigned long quals_unified_in_compaction;
  unsigned long quals_killed;
  unsigned long qual_edges_created;
  unsigned long summary_edges_created;

  unsigned long find_bak_summary_depth;
  unsigned long _find_bak_summary_depth;
  unsigned long find_fwd_summary_depth;
  unsigned long _find_fwd_summary_depth;

  unsigned long find_bak_summary_depth_max;
  unsigned long _find_bak_summary_depth_max;
  unsigned long find_fwd_summary_depth_max;
  unsigned long _find_fwd_summary_depth_max;

  struct Qual_graph_size pre_finish, post_finish;
  struct Qual_graph_size pre_compaction, post_compaction, final;
};

void graph_size(struct Qual_graph_size *gs);

extern struct Qual_stats qual_stats;

extern void Qual_print_stats(void);
extern void Qual_print_sizes(void);

#endif
