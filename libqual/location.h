/* see License.txt for copyright and terms of use */

#ifndef LOCATION_H
#define LOCATION_H

#include "libqual/bool.h"
#include "libqual/libqual.h"
#include "libqual/serialize.h"

// whether to use the SourceLocManager from smbase.
#define USE_SMBASE_SRCLOC 1

#if defined USE_SMBASE_SRCLOC

// see location_smbase_iface.cc

// "location" must be the same size as smbase::SrcLoc, however it is used
// opaquely so the actual type doesn't matter.  smbase::SrcLoc is really an
// enum; we typedef it as long to avoid including all that C++ code.
//
// quarl 2006-11-12
//     On both IA-32 and IA-64, "int" and "enum" are both 32 bits.
typedef int location;
#define NO_LOCATION (-2)

const char *location_filename(location loc);
// unsigned long location_file_offset(location loc);
unsigned long location_line_number(location loc);
unsigned long location_column_number(location loc);

static inline int location_cmp(location loc1, location loc2)
{
  /* abstraction violation -- this should be in srcloc.h */
  return loc1 - loc2;
}

// static inline int /*unsigned long*/ location_index(location loc) {
//   return loc->location_index;
// }

void init_location(void);
void reset_location(void);

// location make_location(int srcloc, int index);

extern s18n_type location_type;

#else

#include "libqual/typed_set.h"

typedef struct Location {
  char *filename;
  unsigned long lineno;
  unsigned long filepos; /* Offset in file */
} *location;

extern location unknown_location;

extern s18n_type location_type;

unsigned long location_hash(location loc);
bool location_eq(location loc1, location loc2);
int location_cmp(location loc1, location loc2);

const char *location_filename(location loc);
unsigned long location_file_offset(location loc);
unsigned long location_line_number(location loc);
unsigned long location_column_number(location loc);

/* Append the line number from loc to name, overwriting any line numbers
   that have already been appended. */
const char *name_with_loc(region r, const char *name, location loc);

/* Set the original .c file that a preprocessed output
   file was derived from. */
void file_set_orig_name(const char *name, const char *orig_name);
const char *file_get_orig_name(const char *name);

void init_location(void);
void reset_location(void);

DEFINE_SET(loc_set, location, location_cmp);
#define scan_loc_set(var, scanner, set) \
for (loc_set_scan(set, &scanner), var = loc_set_next(&scanner); \
     var; \
     var = loc_set_next(&scanner))

#define NO_LOCATION NULL

#endif

#endif
