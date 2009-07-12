/* see License.txt for copyright and terms of use */

#include "libqual/serialize.h"
#include "libqual/utils.h"
#include "libqual/typed_hashset.h"
#include "libqual/typed_map.h"
#include "libqual/typed_bag.h"
#include "libregion/cqual-stdint.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


/* For debugging deserialization failures */
void dummy(void) {}
#define s18nd_return(r) do { int ___x = r; if (___x < 0) dummy(); return ___x; } while(0)


/*************************************
 * Data common to all s18n_contexts
 *************************************/

/* The set of data types understood by the s18n engine */
DEFINE_MAP(string_type_map, const char*, s18n_type*, string_hash, string_eq);
#define scan_string_type_map(kvar, dvar, scanner, map) \
for (string_type_map_scan(map, &scanner); \
     string_type_map_next(&scanner, &kvar, &dvar);)

static region s18n_region = NULL;
static string_type_map registered_types;


/*************************************
 * Data structures for a single s18n_context
 *************************************/

DEFINE_HASHSET(pointer_hashset,ptr_cmp,ptr_hash,void*);
DEFINE_MAP(pointer_map,void*,void*,ptr_hash,ptr_eq);

typedef struct key_value_insertion_t key_value_insertion;
DEFINE_BAG(htkvi_bag, key_value_insertion *);

#define S18N_OUTPUT_BUFFER_LEN 4096

#define S18N_MOST_DESERIALIZED_OBJECTS_SIZE (1<<17)

struct s18n_context_t {
  int io_custom;

  region r;

  pointer_hashset serialized_objects;

  /* Ugly hack to save memory */
  void * most_deserialized_objects_ids[S18N_MOST_DESERIALIZED_OBJECTS_SIZE];
  void * most_deserialized_objects_datas[S18N_MOST_DESERIALIZED_OBJECTS_SIZE];
  pointer_map deserialized_objects;

  /* This should be generalized to some generic callback. */
  htkvi_bag htkvi_closures;

  // union

  // for io_custom=0:
  int fd;
  /* To prevent many small writes, which are slow */
  int buf_len;
  char output_buffer[S18N_OUTPUT_BUFFER_LEN];

  // for io_custom=1:
  void* stream;
  int (*readf)(void*,void*,int);
  int (*writef)(void*,void*,int);
};

/******************************************
 * Creating and destroying contexts
 ******************************************/

s18n_context * s18n_new_context(int fd)
{
  s18n_context * sc;

  sc = (s18n_context*)malloc(sizeof(*sc));
  if (sc == NULL)
    return NULL;

  sc->io_custom = 0;

  sc->fd = fd;
  sc->r = newregion();
  sc->serialized_objects = empty_pointer_hashset(NULL /*sc->r*/);
  memset(sc->most_deserialized_objects_ids, 0,
	 sizeof sc->most_deserialized_objects_ids);
  sc->deserialized_objects = make_pointer_map(sc->r, 2);
  sc->htkvi_closures = empty_htkvi_bag(sc->r);

  sc->buf_len = 0;

  return sc;
}

s18n_context * s18n_new_context_io_custom(void* stream,
                                          int (*readf)(void*,void*,int),
                                          int (*writef)(void*,void*,int))
{
  s18n_context * sc;

  sc = (s18n_context*)malloc(sizeof(*sc));
  if (sc == NULL)
    return NULL;

  sc->io_custom = 1;
  sc->stream = stream;
  sc->readf = readf;
  sc->writef = writef;

  sc->r = newregion();
  sc->serialized_objects = empty_pointer_hashset(NULL /*sc->r*/);
  memset(sc->most_deserialized_objects_ids, 0,
	 sizeof sc->most_deserialized_objects_ids);
  sc->deserialized_objects = make_pointer_map(sc->r, 2);
  sc->htkvi_closures = empty_htkvi_bag(sc->r);

  return sc;
}


void s18n_destroy_context(s18n_context *sc)
{
  if (sc->io_custom==0 && sc->buf_len != 0)
    write(sc->fd, sc->output_buffer, sc->buf_len);

  deleteregion(sc->r);
  destroy_pointer_hashset(sc->serialized_objects);
  free(sc);
}

/******************************************
 * Type management
 ******************************************/

/* Register a new serialization data type */
int s18n_register_type(s18n_type *t)
{
  s18n_type *oldt;

  assert (strlen(t->name) == 8);

  if (string_type_map_lookup(registered_types, t->name, &oldt)) {
    if (oldt != t) {
      fprintf(stderr,
	      "INTERNAL ERROR: Tried to register "
	      "multiple versions of serialization type '%s'\n",
	      t->name);
      abort();
    }
    return 0;
  }

  string_type_map_insert(registered_types, t->name, t);
  if (t->register_children)
    t->register_children(t);

  return 1;
}

/******************************************
 * Utilities for creating type serializers
 ******************************************/

/* Record that we serialized a certain object */
void s18n_record_serialized(s18n_context *sc, void *id)
{
  pointer_hashset_insert(&sc->serialized_objects, id);
}

#define MAX_STEPS (10)

/* Record the mapping from old id to new memory location */
void s18n_record_deserialized(s18n_context *sc, s18n_type *t,
			      void *oldid, void *newid)
{
  int i;
  int phash;

  phash = ptr_hash(oldid) % S18N_MOST_DESERIALIZED_OBJECTS_SIZE;
  for (i = 0; i < MAX_STEPS; i++) {
    if (sc->most_deserialized_objects_ids[phash] == NULL) {
      sc->most_deserialized_objects_ids[phash] = oldid;
      sc->most_deserialized_objects_datas[phash] = newid;
      return;
    } else {
      phash = (phash + 1) % S18N_MOST_DESERIALIZED_OBJECTS_SIZE;
    }
  }

  pointer_map_insert(sc->deserialized_objects, oldid, newid);
}

int s18n_lookup_deserialized(s18n_context *sc, void *oldid, void **newid)
{
  int i;
  int phash;

  phash = ptr_hash(oldid) % S18N_MOST_DESERIALIZED_OBJECTS_SIZE;
  for (i = 0; i < MAX_STEPS; i++) {
    if (sc->most_deserialized_objects_ids[phash] == oldid) {
      *newid = sc->most_deserialized_objects_datas[phash];
      return TRUE;
    } else {
      phash = (phash + 1) % S18N_MOST_DESERIALIZED_OBJECTS_SIZE;
    }
  }

  return pointer_map_lookup(sc->deserialized_objects, oldid, newid);
}

int s18n_lookup_serialized(s18n_context *sc, void *data)
{
  return pointer_hashset_member(sc->serialized_objects, data);
}

int s18n_write(s18n_context *sc, void *buf, int len)
{
  if (sc->io_custom) {
    return ( len == (sc->writef) (sc->stream, buf, len) ) ? len : -EIO;
  } else {
    if (sc->buf_len + len < S18N_OUTPUT_BUFFER_LEN)
      {
        memcpy(sc->output_buffer + sc->buf_len, buf, len);
        sc->buf_len += len;
        return len;
      }

    if (write(sc->fd, sc->output_buffer, sc->buf_len) != sc->buf_len)
      return -EIO;
    sc->buf_len = 0;

    if (len > S18N_OUTPUT_BUFFER_LEN)
      {
        if (write(sc->fd, buf, len) != len)
          return -EIO;
        else
          return len;
      }

    memcpy(sc->output_buffer, buf, len);
    sc->buf_len += len;
    return len;
  }
}

int s18n_read(s18n_context *sc, void *buf, int len)
{
  if (sc->io_custom) {
    return (sc->readf) (sc->stream, buf, len);
  } else {
    return read(sc->fd, buf, len);
  }
}

/******************************************
 * String serialization
 ******************************************/

// TODO: write the pointer value and only serialize it once.
static int string_serialize(s18n_context *sc, s18n_type *t,
			    void *data, void *args)
{
  char *s = data;
  // TODO: use uint16_t
  int32_t len = strlen(s);
  int ret;

  ret = s18n_write(sc, &len, sizeof(len));
  if (ret == sizeof(len))
    ret = s18n_write(sc, s, len);

  if (ret != len)
    return ret < 0 ? ret : -EIO;

  s18n_record_serialized(sc, data);

  return sizeof(len) + len;
}

// TODO: only deserialize once (consistent with above)
// TODO: uint16_t (consistent with above)
static int string_deserialize(s18n_context *sc, s18n_type *t,
			      void *id, void *newdata, void *args)
{
  int32_t len;
  char *s;
  int ret;
  intptr_t count;

  ret = s18n_read(sc, &len, sizeof(len));
  if (ret != sizeof(len))
    s18nd_return(-EIO);

  s = (char *)malloc(len+1);
  if (s == NULL)
    s18nd_return(-ENOMEM);

  ret = s18n_read(sc, s, len);
  if (ret != len) {
    free(s);
    s18nd_return(-EIO);
  }
  s[len] = '\0';

  count = (intptr_t)t->private_info;
  count++;
  t->private_info = (void *)count;

  s18n_record_deserialized(sc, t, id, s);

  *(char **)newdata = s;

  s18nd_return(0);
}

s18n_type s18n_c_string_type = {
  .name = "_cstring",
  .serialize = string_serialize,
  .deserialize = string_deserialize,
  .private_info = NULL
};

/******************************************
 * Serialization for opaque pointers, i.e.  pointers that we don't
 * follow for serialization, but need fixing up.
 ******************************************/

static int opaque_pointer_serialize(s18n_context *sc, s18n_type *t,
				    void *data, void *args)
{
  return 0;
}

static int opaque_pointer_deserialize(s18n_context *sc, s18n_type *t,
				      void *id, void *newdata, void *args)
{
  int result = s18n_lookup_deserialized(sc, id, (void**)newdata);
  assert(result);
  s18nd_return(0);
}

s18n_type s18n_opaque_pointer_type = {
  .name = "_opaquep",
  .serialize = opaque_pointer_serialize,
  .deserialize = opaque_pointer_deserialize,
  .private_info = NULL
};

/******************************************
 * Serialization for nulled pointers, i.e.  pointers that
 * don't get serialized and so should be set to NULL on
 * deserialization.
 ******************************************/

static int nulled_pointer_serialize(s18n_context *sc, s18n_type *t,
				    void *data, void *args)
{
  s18n_record_serialized(sc, data);
  return 0;
}

static int nulled_pointer_deserialize(s18n_context *sc, s18n_type *t,
				      void *id, void *newdata, void *args)
{
  s18n_record_deserialized(sc, t, id, NULL);
  *(void **)newdata = NULL;
  s18nd_return(0);
}

s18n_type s18n_nulled_pointer_type = {
  .name = "_nulledp",
  .serialize = nulled_pointer_serialize,
  .deserialize = nulled_pointer_deserialize,
  .private_info = NULL
};

/*************************************
 * plain struct serialization
 *************************************/

int s18n_generic_struct_serialize(s18n_context *sc, s18n_type *t,
				  void *data, void *args)
{
  int i;
  s18n_generic_struct_type_info *structinfo =
    (s18n_generic_struct_type_info*)t->private_info;
  int ret;

  // {
  //   int valgrind = 0;
  //   if (structinfo->size == 0) valgrind = 1;
  //   for (i=0; i < structinfo->size; ++i) {
  //     if (  ((char*)data)[i] == 0) valgrind = 1;
  //   }
  // }

  ret = s18n_write(sc, data, structinfo->size);
  if (ret != structinfo->size)
    return ret < 0 ? ret : -EIO;

  s18n_record_serialized(sc, data);

  for (i = 0; i < structinfo->num_fields; i++) {
    ret = s18n_serialize(sc, structinfo->fields[i].type,
			 *(void **)(data + structinfo->fields[i].offset));
    if (ret < 0)
      return ret;
  }

  return 0;
}

int s18n_generic_struct_deserialize(s18n_context *sc, s18n_type *t,
				    void *id, void *newdata, void *args)
{
  int i;
  s18n_generic_struct_type_info *structinfo =
    (s18n_generic_struct_type_info*)t->private_info;
  int ret;
  void *data;
  void **fptr;

  if (structinfo->allocator) {
    data = structinfo->allocator(* structinfo->allocator_data, structinfo->size);
  } else {
    data = malloc(structinfo->size);
  }
  if (data == NULL)
    s18nd_return(-ENOMEM);
  ret = s18n_read(sc, data, structinfo->size);
  if (ret != structinfo->size) {
    free(data);
    s18nd_return(-ENOMEM);
  }

  structinfo->alloced_bytes += structinfo->size;

  s18n_record_deserialized(sc, t, id, data);

  for (i = 0; i < structinfo->num_fields; i++)
    {
      fptr = (void **)(data + structinfo->fields[i].offset);
      if (*fptr && !s18n_lookup_deserialized(sc, *fptr, fptr))
	{
	  if (_s18n_deserialize(sc, structinfo->fields[i].type, fptr) < 0)
	    {
              if (structinfo->allocator) {
                // TODO: deallocator
              } else {
                free(data);
              }
	      s18nd_return(-EIO);
	    }
	}
    }
  /* s18n_add_pointer_deserialization_fixup(sc, data +
     structinfo->fields[i].offset); */

  *(void **)newdata = data;
  s18nd_return(0);
}

void s18n_generic_struct_register_children(s18n_type *t)
{
  int i;
  s18n_generic_struct_type_info *structinfo =
    (s18n_generic_struct_type_info*)t->private_info;

  for (i = 0; i < structinfo->num_fields; i++)
    s18n_register_type(structinfo->fields[i].type);
}

#if 1
static void s18n_generic_struct_mem_profile(void)
{
  string_type_map_scanner stms;
  const char *name;
  s18n_type *t;
  s18n_generic_struct_type_info *structinfo;
  int total = 0;

  scan_string_type_map(name, t, stms, registered_types)
    {
      if (t->deserialize == &s18n_generic_struct_deserialize)
	{
	  structinfo = (s18n_generic_struct_type_info*)t->private_info;
	  printf("%s: %12d\n", t->name, structinfo->alloced_bytes);
	  total += structinfo->alloced_bytes;
	}
    }
  printf("Total   : %12d\n", total);

  printf("CStrings: %12ld\n", (long) (intptr_t)s18n_c_string_type.private_info);
}
#endif

/******************************************
 * Opaque tagged structures
 ******************************************/

int s18n_opaque_tagged_type_serialize(s18n_context *sc, s18n_type *t,
				      void *data, void *args)
{
  s18n_opaque_tagged_struct_info *sotsi =
    (s18n_opaque_tagged_struct_info *)t->private_info;
  s18n_type *dynamic_type;

  assert(sotsi != NULL && sotsi->f != NULL);
  dynamic_type = sotsi->f(t, data, args, sotsi->arg);
  assert(dynamic_type != NULL);
  return s18n_serialize(sc, dynamic_type, data);
}

int s18n_opaque_tagged_type_deserialize(s18n_context *sc, s18n_type *t,
					void *id, void *newdata, void *args)
{
  void *newid;
  assert(!s18n_lookup_deserialized(sc, id, &newid));
  s18nd_return(_s18n_deserialize(sc, NULL, newdata));
}

void s18n_opaque_tagged_type_register_children(s18n_type *t)
{
  s18n_opaque_tagged_struct_info *sotsi =
    (s18n_opaque_tagged_struct_info *)t->private_info;

  if (sotsi->register_children)
    sotsi->register_children(t, sotsi->arg);
}

/******************************************
 * Enumerated tagged structures
 ******************************************/

s18n_type * s18n_enum_tagged_typer(s18n_type *t, void *data, void *args, void *arg)
{
  s18n_enum_tagged_struct_info * tsi = (s18n_enum_tagged_struct_info *)arg;
  int64_t tag = tsi->tag_get(data);
  int i;

  for (i = 0; i < tsi->num_tag_vals; i++)
    if (tag == tsi->types[i].tag)
      return tsi->types[i].type;

  return tsi->default_type;
}

void s18n_enum_tag_type_register_children(s18n_type *t, void *arg)
{
  s18n_enum_tagged_struct_info * tsi = (s18n_enum_tagged_struct_info *)arg;
  int i;

  for (i = 0; i < tsi->num_tag_vals; i++)
    s18n_register_type(tsi->types[i].type);

  if (tsi->default_type)
    s18n_register_type(tsi->default_type);
}

/***************************************************
 * For building s18n_types for containers
 ***************************************************/

int s18n_container_serialize(s18n_context *sc, s18n_type *t,
			     void *data, void *args)
{
  s18n_container_info *ci = (s18n_container_info*)t->private_info;
  void *elt;
  void *iterator;
  int ret = 0;
  int nelts;
  void **ids;
  int i;

  s18n_record_serialized(sc, data);

  /* Count the elements */
  nelts = 0;
  ci->ops->iterator(t, data, &iterator, args);
  while ((elt = ci->ops->next(t, data, &iterator, args)) != NULL) {
    nelts++;
  }
  if (s18n_write(sc, &nelts, sizeof(nelts)) != sizeof(nelts))
    return -EIO;

  /* Make an array of their ids and write it to disk */
  ids = (void **)malloc(nelts * sizeof (*ids));
  if (ids == NULL)
    return -ENOMEM;
  ci->ops->iterator(t, data, &iterator, args);
  for (i = 0; (elt = ci->ops->next(t, data, &iterator, args)) != NULL; i++) {
    ids[i] = elt;
  }
  ret = s18n_write(sc, ids, nelts * sizeof (*ids));
  free(ids);
  if (ret != nelts * sizeof (*ids))
    return -EIO;

  /* Serialize the elements */
  ci->ops->iterator(t, data, &iterator, args);
  while ((elt = ci->ops->next(t, data, &iterator, args)) != NULL) {
    ret = s18n_serialize(sc, ci->element_type, elt);
    if (ret < 0)
      return ret;
  }

  return ret;
}

int s18n_container_deserialize(s18n_context *sc, s18n_type *t,
			       void *id, void *newdata, void *args)
{
  s18n_container_info *ci = (s18n_container_info*)t->private_info;
  void *container;
  int nelts;
  int i;
  void *elt;
  void **ids;
  int ret = 0;

  /* How many elements? */
  if (s18n_read(sc, &nelts, sizeof(nelts)) != sizeof(nelts))
    s18nd_return(-EIO);

  /* Create the container */
  container = ci->ops->constructor(t, nelts, args);
  if (container == NULL)
    s18nd_return(-ENOMEM);
  s18n_record_deserialized(sc, t, id, container);

  /* What are their ids? */
  ids = (void **)malloc(nelts * sizeof (*ids));
  if (ids == NULL)
    s18nd_return(-ENOMEM);
  if (s18n_read(sc, ids, nelts * sizeof (*ids)) != nelts * sizeof (*ids))
    {
      ret = -EIO;
      goto done;
    }

  for (i = 0; i < nelts; i++)
    {
      if (!s18n_lookup_deserialized(sc, ids[i], &elt))
	{
	  if (_s18n_deserialize(sc, ci->element_type, &elt) < 0)
	    {
	      ret = -EIO;
	      goto done;
	    }
	}
	ci->ops->insert(t, container, elt, args);
    }

  *(void **)newdata = container;

 done:
  free(ids);
  s18nd_return(ret);
}

void s18n_container_register_children(s18n_type *t)
{
  s18n_container_info *ci = (s18n_container_info*)t->private_info;
  s18n_register_type(ci->element_type);
}

/***************************************************
 * For building s18n_types for hashtables
 ***************************************************/

struct key_value_insertion_t {
  void *hashtable;
  s18n_type *htt;
  void *key;
  void *value;
};

static void s18n_htkvi_run_closures(s18n_context *sc)
{
  htkvi_bag_scanner hbs;
  key_value_insertion *kdins;
  s18n_hashtable_info *hti;

  htkvi_bag_scan(sc->htkvi_closures, &hbs);
  while ((kdins = htkvi_bag_next(&hbs)) != NULL)
    {
      hti = (s18n_hashtable_info*)kdins->htt->private_info;

      hti->ops->insert(kdins->htt,
		       kdins->hashtable, kdins->key, kdins->value, NULL);
      free(kdins);
    }

  sc->htkvi_closures = empty_htkvi_bag(sc->r);
}

static void s18n_htkvi_add_closure(s18n_context *sc, void *hashtable,
				   s18n_type *htt, void *key, void *value)
{
  key_value_insertion *kdins;

  // TODO: use regions for this
  kdins = (key_value_insertion *)xmalloc(sizeof(*kdins));
  kdins->hashtable = hashtable;
  kdins->htt = htt;
  kdins->key = key;
  kdins->value = value;
  htkvi_bag_insert(sc->r, &sc->htkvi_closures, kdins);
}

int s18n_hashtable_serialize(s18n_context *sc, s18n_type *t,
			     void *data, void *args)
{
  s18n_hashtable_info *ci = (s18n_hashtable_info*)t->private_info;
  void *key;
  void *val;
  void *iterator;
  int ret = 0;
  int nelts;
  void **keys;
  void **vals;
  int i;

  s18n_record_serialized(sc, data);

  /* Count the entries */
  nelts = ci->ops->count(t, data);
  // ci->ops->iterator(t, data, &iterator, args);
  // while (ci->ops->next(t, data, &iterator, &key, &val, args)) {
  //   nelts++;
  // }
  if (s18n_write(sc, &nelts, sizeof(nelts)) != sizeof(nelts))
    return -EIO;

  /* Make arrays of keys and vals. */
  keys = (void **)malloc(nelts * sizeof (*keys));
  if (keys == NULL)
    return -ENOMEM;
  vals = (void **)malloc(nelts * sizeof (*vals));
  if (vals == NULL) {
    free(keys);
    return -ENOMEM;
  }
  ci->ops->iterator(t, data, &iterator, args);
  for (i = 0; ci->ops->next(t, data, &iterator, &key, &val, args); i++) {
    keys[i] = key;
    vals[i] = val;
  }
  /* Write them to disk */
  ret = s18n_write(sc, keys, nelts * sizeof (*keys));
  free(keys);
  if (ret != nelts * sizeof (*keys)) {
    free(vals);
    return -EIO;
  }
  ret = s18n_write(sc, vals, nelts * sizeof (*vals));
  free(vals);
  if (ret != nelts * sizeof (*vals))
    return -EIO;

  /* Serialize the elements */
  ci->ops->iterator(t, data, &iterator, args);
  while (ci->ops->next(t, data, &iterator, &key, &val, args)) {
    ret = s18n_serialize(sc, ci->key_type, key);
    if (ret < 0)
      return ret;

    ret = s18n_serialize(sc, ci->value_type, val);
    if (ret < 0)
      return ret;
  }

  return ret;
}

int s18n_hashtable_deserialize(s18n_context *sc, s18n_type *t,
			       void *id, void *newdata, void *args)
{
  s18n_hashtable_info *ci = (s18n_hashtable_info*)t->private_info;
  void *hashtable;
  int nelts;
  int i;
  void *key;
  void *val;
  void **keys;
  void **vals;
  int ret = 0;

  /* How many elements? */
  if (s18n_read(sc, &nelts, sizeof(nelts)) != sizeof(nelts))
    s18nd_return(-EIO);

  /* Create the hashtable */
  hashtable = ci->ops->constructor(t, nelts, args);
  if (hashtable == NULL)
    s18nd_return(-ENOMEM);
  s18n_record_deserialized(sc, t, id, hashtable);

  /* Load arrays of their ids */
  keys = (void **)malloc(nelts * sizeof (*keys));
  if (keys == NULL)
    s18nd_return(-ENOMEM);
  vals = (void **)malloc(nelts * sizeof (*vals));
  if (vals == NULL) {
    free(keys);
    s18nd_return(-ENOMEM);
  }
  if (s18n_read(sc, keys, nelts * sizeof (*keys)) != nelts * sizeof (*keys))
    {
      ret = -EIO;
      goto done;
    }
  if (s18n_read(sc, vals, nelts * sizeof (*vals)) != nelts * sizeof (*vals))
    {
      ret = -EIO;
      goto done;
    }

  for (i = 0; i < nelts; i++)
    {
      if (!s18n_lookup_deserialized(sc, keys[i], &key))
	{
	  if (_s18n_deserialize(sc, ci->key_type, &key) < 0)
	    {
	      ret = -EIO;
	      goto done;
	    }
	}
      if (!s18n_lookup_deserialized(sc, vals[i], &val))
	{
	  if (_s18n_deserialize(sc, ci->value_type, &val) < 0)
	    {
	      ret = -EIO;
	      goto done;
	    }
	}
      /* ci->ops->insert(t, hashtable, key, val, args); */
      s18n_htkvi_add_closure(sc, hashtable, t, key, val);
    }

  *(void **)newdata = hashtable;

 done:
  free(keys);
  free(vals);
  s18nd_return(ret);
}

void s18n_hashtable_register_children(s18n_type *t)
{
  s18n_hashtable_info *hti = (s18n_hashtable_info*)t->private_info;
  s18n_register_type(hti->key_type);
  s18n_register_type(hti->value_type);
}

/******************************************
 * Hashsets
 *****************************************/

int s18n_hashset_table_serialize(s18n_context *sc, s18n_type *t,
                                 void *data, void *args)
{
  s18n_hashtable_info *ci = (s18n_hashtable_info*)t->private_info;
  void *key;
  void *iterator;
  int ret = 0;
  int nelts;
  void **keys;
  int i;

  s18n_record_serialized(sc, data);

  /* Count the entries */
  nelts = ci->ops->count(t, data);
  if (s18n_write(sc, &nelts, sizeof(nelts)) != sizeof(nelts))
    return -EIO;

  /* Make arrays of keys. */
  // TODO: don't create arrays; just write N*{pointer,value}
  keys = (void **)malloc(nelts * sizeof (*keys));
  if (keys == NULL)
    return -ENOMEM;
  ci->ops->iterator(t, data, &iterator, args);
  for (i = 0; ci->ops->next(t, data, &iterator, &key, NULL, args); i++) {
    keys[i] = key;
  }
  /* Write them to disk */
  ret = s18n_write(sc, keys, nelts * sizeof (*keys));
  free(keys);
  if (ret != nelts * sizeof (*keys)) {
    return -EIO;
  }

  /* Serialize the elements */
  ci->ops->iterator(t, data, &iterator, args);
  while (ci->ops->next(t, data, &iterator, &key, NULL, args)) {
    ret = s18n_serialize(sc, ci->key_type, key);
    if (ret < 0)
      return ret;
  }

  return ret;
}

int s18n_hashset_table_deserialize(s18n_context *sc, s18n_type *t,
			       void *id, void *newdata, void *args)
{
  s18n_hashtable_info *ci = (s18n_hashtable_info*)t->private_info;
  void *hashset_table;
  int nelts;
  int i;
  void *key;
  void **keys;
  int ret = 0;

  /* How many elements? */
  if (s18n_read(sc, &nelts, sizeof(nelts)) != sizeof(nelts))
    s18nd_return(-EIO);

  /* Create the hashset_table */
  hashset_table = ci->ops->constructor(t, nelts, args);
  if (hashset_table == NULL)
    s18nd_return(-ENOMEM);
  s18n_record_deserialized(sc, t, id, hashset_table);

  /* Load arrays of their ids */
  keys = (void **)malloc(nelts * sizeof (*keys));
  if (keys == NULL)
    s18nd_return(-ENOMEM);
  if (s18n_read(sc, keys, nelts * sizeof (*keys)) != nelts * sizeof (*keys))
    {
      ret = -EIO;
      goto done;
    }

  for (i = 0; i < nelts; i++)
    {
      if (!s18n_lookup_deserialized(sc, keys[i], &key))
	{
	  if (_s18n_deserialize(sc, ci->key_type, &key) < 0)
	    {
	      ret = -EIO;
	      goto done;
	    }
	}
      /* ci->ops->insert(t, hashset_table, key, val, args); */
      s18n_htkvi_add_closure(sc, hashset_table, t, key, NULL);
    }

  *(void **)newdata = hashset_table;

 done:
  free(keys);
  s18nd_return(ret);
}

void s18n_hashset_table_register_children(s18n_type *t)
{
  s18n_hashtable_info *hti = (s18n_hashtable_info*)t->private_info;
  s18n_register_type(hti->key_type);
}


/******************************************
 * Serialization driver code
 ******************************************/

/* Serialize some data */
int s18n_serialize(s18n_context *sc, s18n_type *t, void *data)
{
  int ret;

  if (data == NULL)
    return 0;

  if (pointer_hashset_member(sc->serialized_objects, data))
    return 0;

  ret = s18n_write(sc, t->name, 8);
  if (ret != 8)
    return ret < 0 ? ret : -EIO;

  // write id as pointer address
  ret = s18n_write(sc, &data, sizeof(data));
  if (ret != sizeof(data))
    return ret < 0 ? ret : -EIO;

  return t->serialize(sc, t, data, NULL) >= 0;
}

/* Deserialize some data */
int _s18n_deserialize(s18n_context *sc, s18n_type *expected_type, void *newdata)
{
  char type_name[9];
  void *id;
  s18n_type *t;
  int ret;

  ret = s18n_read(sc, type_name, 8);
  if (ret != 8)
    s18nd_return(-EIO);
  type_name[8] = '\0';
  if (expected_type && strncmp(type_name, expected_type->name, 8))
    s18nd_return(-EIO);

  ret = s18n_read(sc, &id, sizeof(id));
  if (ret != sizeof(id))
    s18nd_return(-EIO);

  if (!string_type_map_lookup(registered_types, type_name, &t)) {
    fprintf(stderr,
	    "INTERNAL ERROR: tried to deserialize object of unknown type '%s'\n",
	    type_name);
    s18nd_return(-EIO);
  }

  s18nd_return(t->deserialize(sc, t, id, newdata, NULL));
}

int s18n_deserialize(s18n_context *sc, s18n_type *expected_type, void *newdata)
{
  int ret = _s18n_deserialize(sc, expected_type, newdata);
  if (ret >= 0)
    s18n_htkvi_run_closures(sc);

  s18nd_return(ret);
}

/*****************
 * Must be called first
 *****************/

void init_s18n(void)
{
  assert(s18n_region == NULL);
  s18n_region = newregion();
  registered_types = make_string_type_map(s18n_region, 16);
  s18n_register_type(&s18n_c_string_type);
  s18n_register_type(&s18n_opaque_pointer_type);

  if (0)
    atexit(s18n_generic_struct_mem_profile);
}

void reset_s18n(void)
{
  if (s18n_region) {
    deleteregion(s18n_region);
    s18n_region = NULL;
  }
}
