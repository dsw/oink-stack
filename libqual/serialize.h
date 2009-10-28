/* see License.txt for copyright and terms of use */

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdlib.h>
#include "libregion/cqual-stdint.h"

/*****************
 * Must be called first
 *****************/

void init_s18n(void);

void reset_s18n(void);

/******************************************
 * All the metadata about a single (de)serialization is stored here.
 ******************************************/

typedef struct s18n_context_t s18n_context;

/* Begin/end a (de)serialization */
s18n_context * s18n_new_context(int fd);
s18n_context * s18n_new_context_io_custom(void* stream, int (*readf)(void*,void*,int), int (*writef)(void*,const void*,int));
void s18n_destroy_context(s18n_context *sc);

/***********************************************
 * Describes a serializable data type.
 ***********************************************/

typedef struct s18n_type_t s18n_type;

/* Interface to functions for (de)serializing a single data item */
typedef int (*s18n_serialize_func)(s18n_context *sc, s18n_type *t,
				   void *data, void *args);
typedef int (*s18n_deserialize_func)(s18n_context *sc, s18n_type *t,
					void * id, void *newdata, void *args);
typedef void (*s18n_register_func)(s18n_type *t);

/* Describes a single data type for serialization */
struct s18n_type_t {
  const char *name; /* Must be exactly 8 characters and globally unique */

  s18n_serialize_func serialize;
  s18n_deserialize_func deserialize;
  s18n_register_func register_children;

  void *private_info;
};

/*****************************************
 * Utilities for creating new s18n types
 *****************************************/

/* Register a new serialization data type */
int s18n_register_type(s18n_type *t);

/* int s18n_context_fd(s18n_context *sc); */

/* Record that we serialized a certain object */
void s18n_record_serialized(s18n_context *sc, void *id);

/* Record the mapping from old id to new memory location */
void s18n_record_deserialized(s18n_context *sc, s18n_type *t, void *oldid, void *newid);

/* Find the new address of a deserialized object */
int s18n_lookup_deserialized(s18n_context *sc, void *oldid, void **newid);

/* Find out whether data has been serialized */
int s18n_lookup_serialized(s18n_context *sc, void *data);

/* For use by new deserializers only! */
int _s18n_deserialize(s18n_context *sc, s18n_type *expected_type, void *newdata);

int s18n_write(s18n_context *sc, const void *buf, int len);
int s18n_read(s18n_context *sc, void *buf, int len);

/********************************
 * s18n_types for basic c types
 ********************************/

/* For (de)serializing regular C strings */
extern s18n_type s18n_c_string_type;

/* For pointers that shouldn't be followed, just fixed up on
   deserialization. */
extern s18n_type s18n_opaque_pointer_type;

/* For pointers that shouldn't be followed, but should be
   set to NULL in the deserialized object. */
extern s18n_type s18n_nulled_pointer_type;

/**********************************************************
 * Utilities for building s18n_types for normal structures
 **********************************************************/

#define s18n_offsetof(type,field) ((int)((char *)(&(((type *)NULL)->field))))
#define s18n_sizeof(type, field) (sizeof(((type *)NULL)->field))

/* Describes the type of a single field of a struct */
typedef struct s18n_field_t {
  int offset;
  s18n_type *type;
} s18n_field;


typedef void *(*s18n_allocator_f)(void *data, size_t size);

typedef struct s18n_generic_struct_type_info_t {
  s18n_allocator_f allocator;
  void **allocator_data;
  int size;
  int num_fields;
  s18n_field *fields;
  int alloced_bytes;
} s18n_generic_struct_type_info;

int s18n_generic_struct_serialize(s18n_context *sc, s18n_type *t,
				  void *data, void *args);
int s18n_generic_struct_deserialize(s18n_context *sc, s18n_type *t,
				       void *id, void *newdata, void *args);
void s18n_generic_struct_register_children(s18n_type *t);

#define S18N_FIELD(s,f,t) { s18n_offsetof(s,f), &t }
#define S18N_GENERIC_STRUCT_BEGIN_A(tname,id,a,ad,sname,nfields)    \
s18n_type tname = { \
  .name         = #id, \
  .serialize    = s18n_generic_struct_serialize, \
  .deserialize  = s18n_generic_struct_deserialize, \
  .register_children = s18n_generic_struct_register_children, \
  .private_info = &(s18n_generic_struct_type_info) { \
    .allocator      = (s18n_allocator_f) a, \
    .allocator_data = (void**) ad, \
    .size           = sizeof(sname), \
    .num_fields     = nfields, \
    .fields = (s18n_field[]){
#define S18N_GENERIC_STRUCT_END }}};
#define S18N_GENERIC_STRUCT_BEGIN(tname,id,sname,nfields) \
  S18N_GENERIC_STRUCT_BEGIN_A(tname,id,NULL,NULL,sname,nfields)

/**********************************************
 * For building s18n_types for unions/subtyped structures, where
 * the type of a specific structure is determined by a function.
 **********************************************/

typedef s18n_type * (*dynamic_typer)(s18n_type *t, void *data,
				     void *args, void *arg);
typedef void (*opaque_register_func)(s18n_type *t, void *arg);

typedef struct s18n_opaque_tagged_struct_info_t {
  dynamic_typer f;
  opaque_register_func register_children;
  void *arg;
} s18n_opaque_tagged_struct_info;

int s18n_opaque_tagged_type_serialize(s18n_context *sc, s18n_type *t,
				      void *data, void *args);
int s18n_opaque_tagged_type_deserialize(s18n_context *sc, s18n_type *t,
					   void *data, void *newdata, void *args);
void s18n_opaque_tagged_type_register_children(s18n_type *t);

#define S18N_OPAQUE_TAG_TYPE(tname,id,sname,typerfunc,typer_arg, regchildfn) \
s18n_type tname = { \
  .name = #id, \
  .serialize = s18n_opaque_tagged_type_serialize, \
  .deserialize = s18n_opaque_tagged_type_deserialize, \
  .register_children = s18n_opaque_tagged_type_register_children, \
  .private_info = &(s18n_opaque_tagged_struct_info) { \
    .f = typer, \
    .arg = typer_arg, \
    .register_children = regchildfn \
  } \
};

/**********************************************
 * For building s18n_types for tagged structures (subtyping and
 * unions) distinguished by an enum
 **********************************************/

typedef struct s18n_enum_val_t {
  int64_t tag;
  s18n_type *type;
} s18n_enum_val;

typedef struct s18n_enum_tagged_struct_info_t {
  int64_t (*tag_get)(void*);
  void (*tag_set)(void*, int64_t);
  s18n_type *default_type;
  int num_tag_vals;
  s18n_enum_val *types;
} s18n_enum_tagged_struct_info;

s18n_type * s18n_enum_tagged_typer(s18n_type *t, void *data,
				   void *args, void *arg);
void s18n_enum_tag_type_register_children(s18n_type *t, void *arg);

#define S18N_ENUM_TAG_TYPE_BEGIN(tname,id,sname,tagfield,defaulttype,numtagvals) \
  static int64_t tname##_gettag(void *x) { return ((sname*)x)->tagfield; } \
  static void tname##_settag(void *x, int64_t tag) { ((sname*)x)->tagfield = tag; } \
s18n_type tname = { \
  .name = #id, \
  .serialize = s18n_opaque_tagged_type_serialize, \
  .deserialize = s18n_opaque_tagged_type_deserialize, \
  .register_children = s18n_opaque_tagged_type_register_children, \
  .private_info = &(s18n_opaque_tagged_struct_info) { \
    .f = s18n_enum_tagged_typer, \
    .register_children = s18n_enum_tag_type_register_children, \
    .arg = &(s18n_enum_tagged_struct_info) { \
      .tag_get = &tname##_gettag, \
      .tag_set = &tname##_settag, \
      .default_type = defaulttype, \
      .num_tag_vals = numtagvals, \
      .types = (s18n_enum_val[]){
#define S18N_ENUM_TAG_TYPE_END }}}};

/***************************************************
 * For building s18n_types for containers
 ***************************************************/

typedef struct s18n_container_ops_t {
  void (*iterator)(s18n_type *ct, void *container, void *iterator, void *arg);
  void *(*next)(s18n_type *ct, void *container, void *iterator, void *arg);

  void *(*constructor)(s18n_type *ct, size_t size, void *arg);
  void (*insert)(s18n_type *ct, void *container, void *element, void *arg);
} s18n_container_ops;

typedef struct s18n_container_info_t {
  s18n_container_ops *ops;
  s18n_type *element_type;
  void *private_info;
} s18n_container_info;

int s18n_container_serialize(s18n_context *sc, s18n_type *t,
			     void *data, void *args);
int s18n_container_deserialize(s18n_context *sc, s18n_type *t,
				  void *data, void *newdata, void *args);
void s18n_container_register_children(s18n_type *t);

/***************************************************
 * For building s18n_types for hashtables
 ***************************************************/

typedef struct s18n_hashtable_ops_t {
  void (*iterator)(s18n_type *htt, void *hashtable, void *iterator, void *arg);
  int (*next)(s18n_type *htt, void *hashtable, void *iterator, void *key, void *data, void *arg);

  void *(*constructor)(s18n_type *htt, size_t size, void *arg);
  void (*insert)(s18n_type *htt, void *hashtable, void *key, void *data, void *arg);
  size_t (*count)(s18n_type *htt, void *hashtable);
} s18n_hashtable_ops;

typedef struct s18n_hashtable_info_t {
  s18n_hashtable_ops *ops;
  s18n_type *key_type;
  s18n_type *value_type;
  void *private_info;
} s18n_hashtable_info;

int s18n_hashtable_serialize(s18n_context *sc, s18n_type *t,
			     void *data, void *args);
int s18n_hashtable_deserialize(s18n_context *sc, s18n_type *t,
				  void *data, void *newdata, void *args);
void s18n_hashtable_register_children(s18n_type *t);

int s18n_hashset_table_serialize(s18n_context *sc, s18n_type *t,
			     void *data, void *args);
int s18n_hashset_table_deserialize(s18n_context *sc, s18n_type *t,
				  void *data, void *newdata, void *args);
void s18n_hashset_table_register_children(s18n_type *t);

/*******************************************
 * Functions for actually doing serialization
 *******************************************/

/* Serialize or deserialize some data */
int s18n_serialize(s18n_context *sc, s18n_type *t, void *data);
int s18n_deserialize(s18n_context *sc, s18n_type *expected_type, void *data);

#endif

