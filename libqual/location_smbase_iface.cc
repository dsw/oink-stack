// see License.txt for copyright and terms of use

/* C interface to smbase::SourceLocManager, compatible with old
   cqual::location interface */

// quarl 2006-05-13

#include <cassert>

extern "C" {
#include "libqual/location.h"
#include "libqual/serialize.h"
}
#include "libregion/regions.h"
#include <cerrno>

#if defined (USE_SMBASE_SRCLOC)
#include "smbase/srcloc.h"

#define CPP_JOIN(x,y) CPP_JOIN_1(x,y)
#define CPP_JOIN_1(x,y) x##y

#define static_assert(x) typedef char CPP_JOIN(static_assert_,__LINE__)[(x)?2:-2]

#define s18nd_return(r) return r

// region location_region = NULL;

void init_location(void)
{
  // assert(location_region == NULL);
  // location_region = newregion();
}

void reset_location(void)
{
  // if (location_region) {
  //   deleteregion(location_region);
  //   location_region = NULL;
  // }
}

// long num_locations = 0;

// location make_location(int srcloc, int index)
// {
//   assert(srcloc != -1);
//   location loc = (location) ralloc(location_region, struct Location);
//   loc->srcloc = srcloc;
//   loc->location_index = index;
//   ++num_locations;
//   return loc;
// }

const char *location_filename(location loc)
{
  if (loc == -2) return "<NO_LOCATION>";
  return sourceLocManager->getFile((SourceLoc) loc);
}

unsigned long location_file_offset(location loc)
{
  if (loc == -2) return 0;
  return sourceLocManager->getOffset((SourceLoc) loc);
}

unsigned long location_line_number(location loc)
{
  if (loc == -2) return 0;
  return sourceLocManager->getLine((SourceLoc) loc);
}

unsigned long location_column_number(location loc)
{
  if (loc == -2) return 0;
  return sourceLocManager->getCol((SourceLoc) loc);
}

// quarl 2006-11-12
//     "SourceLoc" and "location" are supposed to be the same data type (but
//     we avoided the include dependency of smbase/srcloc.h).  "void*" is the
//     generic argument type here so the location just has to fit inside it.
static_assert(sizeof(location) <= sizeof(void*));
static_assert(sizeof(SourceLoc) == sizeof(location));

static int srcloc_serialize(s18n_context *sc, s18n_type *t,
                            void *data, void *args)
{
  // quarl 2006-11-13
  //     Convert back to "location" via this pointer type cast.  If we instead
  //     convert to long then to int, we'd be truncating the high-order bits,
  //     but that would not be the right thing depending on endianness.
  //     s18n_generic_struct_serialize() had given us too much data by
  //     treating the offset into the struct as a pointer.
  // SourceLoc loc = (SourceLoc) (location) (long) data;
  SourceLoc loc = (SourceLoc) *( (location*) &data );

  int ret;

  if (loc == SL_UNKNOWN) {      // SL_UNKNOWN = 0
    int32_t x = 0;
    ret = s18n_write(sc, &x, sizeof(x));
    if (ret != sizeof(x))
      return ret < 0 ? ret : -EIO;
    return sizeof(x);
  } else if (loc == SL_INIT) {  // SL_INIT = -1
    int32_t x = -1;
    ret = s18n_write(sc, &x, sizeof(x));
    if (ret != sizeof(x))
      return ret < 0 ? ret : -EIO;
    return sizeof(x);
  } else if ((location)loc == NO_LOCATION) { // NO_LOCATION = -2
    int32_t x = -2;
    ret = s18n_write(sc, &x, sizeof(x));
    if (ret != sizeof(x))
      return ret < 0 ? ret : -EIO;
    return sizeof(x);
  }

  const char *filename = sourceLocManager->getFile(loc, false);
  assert(filename && *filename);
  int32_t len = strlen(filename);
  {
    ret = s18n_write(sc, &len, sizeof(len));
    if (ret != sizeof(len))
      return ret < 0 ? ret : -EIO;

    ret = s18n_write(sc, const_cast<void*>(static_cast<void const*>(filename)),
                     len);
    if (ret != len)
      return ret < 0 ? ret : -EIO;
  }
  int32_t offset = sourceLocManager->getOffset_nohashline(loc);
  {
    ret = s18n_write(sc, &offset, sizeof(offset));
    if (ret != sizeof(offset))
      return ret < 0 ? ret : -EIO;
  }

  // s18n_record_serialized(sc, data);
  return sizeof(len) + len + sizeof(offset);
}

static int srcloc_deserialize(s18n_context *sc, s18n_type *t,
                              void *id, void *newdata, void *args)
{
  int32_t len;

  int ret;

  ret = s18n_read(sc, &len, sizeof(len));
  if (ret != sizeof(len))
    s18nd_return(-EIO);

  if (len == 0) {               // SL_UNKNOWN = 0
    *((location*) newdata) = (location) SL_UNKNOWN;
    s18nd_return(0);
  }

  if (len == -1) {              // SL_INIT = -1
    *((location*) newdata) = (location) SL_INIT;
    s18nd_return(0);
  }

  if (len == -2) {              // NO_LOCATION = -2
    *((location*) newdata) = NO_LOCATION;
    s18nd_return(0);
  }

  assert(len > 0);

  char *filename = (char*)alloca(len+1);
  if (filename == NULL)
    s18nd_return(-ENOMEM);

  ret = s18n_read(sc, filename, len);
  if (ret != len)
    s18nd_return(-EIO);
  filename[len] = '\0';

  int32_t offset;
  ret = s18n_read(sc, &offset, sizeof(offset));
  if (ret != sizeof(offset))
    s18nd_return(-EIO);

  SourceLoc loc = sourceLocManager->encodeOffset(filename, offset);
  *((location*) newdata) = (location) loc;

  // s18n_record_deserialized(sc, t, id, (void*) loc);
  s18nd_return(0);
}

s18n_type location_type = {
  /* .name = */ "srcloc_2",
  /* .serialize = */ srcloc_serialize,
  /* .deserialize = */ srcloc_deserialize,
  /* .private_info = */ NULL
};

// in location.c:

// S18N_GENERIC_STRUCT_BEGIN(location_type, location, struct Location, 1)
//   S18N_FIELD(struct Location, srcloc, srcloc_type)
// S18N_GENERIC_STRUCT_END

#endif
