// $Id$

// quarl 2006-07-11
//    Obstack is a pool allocator.

#include <glibc-fs/prelude.h>
#include <obstack.h>

// struct _obstack_chunk
// {
//     char  *limit;
//     struct _obstack_chunk *prev;
//     char	contents[4];
// };

// struct obstack
// {
//     long	chunk_size;
//     struct _obstack_chunk *chunk;
//     char	*object_base;
//     char	*next_free;
//     char	*chunk_limit;
//     PTR_INT_TYPE temp;
//     int   alignment_mask;

//     struct _obstack_chunk *(*chunkfun) (void *, long);
//     void (*freefun) (void *, struct _obstack_chunk *);
//     void *extra_arg;

//     unsigned use_extra_arg:1;
//     unsigned maybe_empty_object:1;
//     unsigned alloc_failed:1;
// };

void _obstack_newchunk (struct obstack *obstack, int i1) {
    // make sure these are unified
    obstack->object_base = obstack->next_free = obstack->chunk_limit = obstack->extra_arg;
}

void _obstack_free (struct obstack *obstack, void *p1) {
    obstack->freefun(p1, obstack->chunk);
}

int _obstack_begin (struct obstack *obstack, int i1, int i2, void *(*f1) (long), void (*f2) (void *)) {}
int _obstack_begin_1 (struct obstack *obstack, int i2, int i3, void *(*f1) (void *, long), void (*f2) (void *, void *), void *p1) {}
int _obstack_memory_used (struct obstack *obstack) {}

#undef obstack_init
void obstack_init (struct obstack *obstack) {}

#undef obstack_alloc
void * obstack_alloc (struct obstack *obstack, int size) {}

#undef obstack_copy
void * obstack_copy (struct obstack *obstack, const void *address, int size) {
    // actually it should be a unidirectional edge from 'address' to return
    // value instead of a unification, but since memcpy doesn't work we do
    // this for now.
    return (void*) address;
}

#undef obstack_copy0
void * obstack_copy0 (struct obstack *obstack, const void *address, int size) {
    return (void*) address;
}

#undef obstack_free
void obstack_free (struct obstack *obstack, void *block) { _obstack_free(obstack,block); }

#undef obstack_blank
void obstack_blank (struct obstack *obstack, int size) {}

#undef obstack_grow
void obstack_grow (struct obstack *obstack, const void *data, int size) {
    // obstack_grow appends data to the "current" obstack.  Since we can't
    // copy arbitrary data, we instead unify the data pointers.  We use
    // extra_arg since it happens to be a 'void*'.  This may produce false
    // positives.
    obstack->extra_arg = (void*) data;
}

#undef obstack_grow0
void obstack_grow0 (struct obstack *obstack, const void *data, int size) {
    obstack->extra_arg = (void*) data;
}

#undef obstack_1grow
void obstack_1grow (struct obstack *obstack, int data_char) {
    *((int*) obstack->extra_arg) = data_char;
}

#undef obstack_ptr_grow
void obstack_ptr_grow (struct obstack *obstack, const void *data) {
    *((void**) obstack->extra_arg) = (void*) data;
}

#undef obstack_int_grow
void obstack_int_grow (struct obstack *obstack, int data) {
    *((int*) obstack->extra_arg) = data;
}

#undef obstack_finish
void * obstack_finish (struct obstack *obstack) {
    return obstack->extra_arg;
}

#undef obstack_object_size
int obstack_object_size (struct obstack *obstack) {}

#undef obstack_room
int obstack_room (struct obstack *obstack) {}

#undef obstack_make_room
void obstack_make_room(struct obstack *obstack, int size) {}

#undef obstack_1grow_fast
void obstack_1grow_fast (struct obstack *obstack, int data_char) {
    *((int*) obstack->extra_arg) = data_char;
}

#undef obstack_ptr_grow_fast
void obstack_ptr_grow_fast (struct obstack *obstack, const void *data) {
    *((void**) obstack->extra_arg) = (void*) data;
}

#undef obstack_int_grow_fast
void obstack_int_grow_fast (struct obstack *obstack, int data) {
    *((int*) obstack->extra_arg) = data;
}

#undef obstack_blank_fast
void obstack_blank_fast (struct obstack *obstack, int size) {}

#undef obstack_base
void * obstack_base (struct obstack *obstack) {
    return obstack->extra_arg;
}

#undef obstack_next_free
void * obstack_next_free (struct obstack *obstack) {
    return obstack->extra_arg;
}

#undef obstack_alignment_mask
int obstack_alignment_mask (struct obstack *obstack) {}

#undef obstack_chunk_size
int obstack_chunk_size (struct obstack *obstack) {}

#undef obstack_memory_used
int obstack_memory_used (struct obstack *obstack) {}

void (*obstack_alloc_failed_handler) (void);

int obstack_exit_failure;
