/* see License.txt for copyright and terms of use */

#include "libqual/location.h"
#include "libqual/utils.h"
#include "libqual/typed_map.h"
#include <string.h>

// quarl 2006-05-13
//    struct Location used to consist of a BaseLocation and a location_index.
//    This was wasteful since index was replicated outside the location.
//    qualcc is memory-bound by the size of struct Qual_edge, so trimming it
//    by one word saves 15% memory!  As 'location' is now implemented as
//    smbase::SrcLoc, it is also much more efficient if we don't have to wrap
//    a struct pointer around it.

#if defined (USE_SMBASE_SRCLOC)

// extern s18n_type srcloc_type;

// S18N_GENERIC_STRUCT_BEGIN(location_type, location, struct Location, 1)
//   S18N_FIELD(struct Location, srcloc, srcloc_type)
// S18N_GENERIC_STRUCT_END

// see location_smbase_iface.cc

#else

location unknown_location = &(struct Location)
{
  "<unknown>",
  0,
  0
};

unsigned long location_hash(location loc)
{
  unsigned long h;

  h = 0;
  h = loc->lineno;
  h = 33*h + 720 + loc->filepos;
  h = 33*h + 720 + string_hash(loc->filename);
  return h;
}

bool location_eq(location loc1, location loc2)
{
  return (loc1->lineno == loc2->lineno &&
	  loc1->filepos == loc2->filepos &&
	  !strcmp(loc1->filename, loc2->filename));
}

int location_cmp(location loc1, location loc2)
{
  int result;

  result = strcmp(loc1->filename, loc2->filename);
  if (!result)
    result = loc1->filepos - loc2->filepos;
  return result;
}

// int location_index(location loc)
// {
//   return loc->location_index;
// }

const char *location_filename(location loc)
{
  return loc->filename;
}

unsigned long location_file_offset(location loc)
{
  return loc->filepos;
}

unsigned long location_line_number(location loc)
{
  return loc->lineno;
}

unsigned long location_column_number(location loc)
{
  return -1; /* not available */
}

S18N_GENERIC_STRUCT_BEGIN(location_type, location, struct Location, 1)
  S18N_FIELD(struct Location, filename, s18n_c_string_type)
S18N_GENERIC_STRUCT_END

DEFINE_MAP(file_orig_file_map, const char *, const char *, string_hash, string_eq);
#define scan_file_orig_file_map(kvar, dvar, scanner, map) \
for (file_orig_file_map_scan(map, &scanner); \
     file_orig_file_map_next(&scanner, &kvar, &dvar);)

static region file_region = NULL;
static file_orig_file_map file_orig_files = NULL;

/* Set the original .c file that a preprocessed output
   file was derived from. */
void file_set_orig_name(const char *name, const char *orig_name)
{
  insist(file_orig_file_map_insert(file_orig_files, name, orig_name));
}

const char *file_get_orig_name(const char *name)
{
  const char *orig_name;
  assert(file_orig_files);
  if (file_orig_file_map_lookup(file_orig_files, name, &orig_name))
    return orig_name;
  else
    return NULL;
}

/* Append the line number from loc to name, overwriting any line numbers
   that have already been appended. */
const char *name_with_loc(region r, const char *name, location loc) {
  char *index;
  int base_len;
  char *base_name;

  if ((index = strstr(name, "@")))
    base_len = index - name;
  else
    base_len = strlen(name);
  base_name = alloca((base_len + 2) * sizeof(char));
  strncpy(base_name, name, base_len);
  base_name[base_len] = '@';
  base_name[base_len+1] = '\0';
  return rstrcat(r, base_name, inttostr(r, loc->lineno));
}

void init_location(void)
{
  file_region = newregion();
  file_orig_files = make_file_orig_file_map(file_region, 37);
}

void reset_location(void) {
  if (file_region) {
    deleteregion(file_region);
    file_region = NULL;
  }
  file_orig_files = NULL;
}

#endif // !defined (USE_SMBASE_SRCLOC)
