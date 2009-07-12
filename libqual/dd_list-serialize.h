/* see License.txt for copyright and terms of use */

#ifndef DD_LIST_SERIALIZE_H
#define DD_LIST_SERIALIZE_H

/***************
 * The interface between dd_list.h and serialize.h
 ***************/

#include "libqual/dd_list.h"
#include "libqual/serialize.h"

/* Most clients don't need these: just use the standard
   s18n_container_ops dd_list_ops.  These are just available in case you
   need to overload some of them. */
void s18n_dd_list_iterator(s18n_type *ct, void *container, void *iterator, void *arg);
void *s18n_dd_list_next(s18n_type *ct, void *container, void *iterator, void *arg);
void *s18n_dd_list_constructor(s18n_type *ct, size_t size, void *arg);
void s18n_dd_list_insert(s18n_type *ct, void *container, void *element, void *arg);

/* The standard serialization ops for dd_lists */
extern s18n_container_ops dd_list_ops;

/* Predicates on dd_lists */
typedef struct dd_list_predicate_t
{
  dd_cmp_fn f;
  void *arg;
} dd_list_predicate;

/* Describes a specialization of dd_list */
typedef struct dd_list_kind_t
{
  region *r;
  dd_list_predicate *ddlp;
} dd_list_kind;

#endif
