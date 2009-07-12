// see License.txt for copyright and terms of use

#include "libregion/regions.h"
#include "oink_util.h"          // explain_bad_alloc
#include <new>
#include <cstdlib>

// quarl 2006-05-04
//    Set libregion's out-of-memory hook to throw a std::bad_alloc, so we can
//    catch and report in main().

namespace {
  // quarl 2006-05-28
  //    Squat on a safety buffer of memory that we deallocate on
  //    out-of-memory conditions, so that we can cleanup better (otherwise
  //    sometimes we run out of memory while trying to explain how we ran
  //    out of memory).
  // TODO: use this in 'new' handler too.
  struct SafetyMemory {
  private:
    void *safe;

  public:
    SafetyMemory() {
      safe = malloc(100*1024);
    }

    ~SafetyMemory() { free(); }

    void free() {
      std::free(safe);
      safe = NULL;
    }
  };

  SafetyMemory TheSafetyMemory;

  struct LibRegionInitializer {
    LibRegionInitializer() {
      set_nomem_handler(nomem_handler);
    }

  private:
    static void nomem_handler() {
      TheSafetyMemory.free();

      // quarl 2006-05-28
      //    'throw std::bad_alloc()' somehow doesn't always get caught, so
      //    just explain it now and exit.

      // throw std::bad_alloc();
      explain_bad_alloc();      // calls abort()
    }
  };

  LibRegionInitializer TheLibRegionInitializer;
}
