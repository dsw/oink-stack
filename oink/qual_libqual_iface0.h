// see License.txt for copyright and terms of use

// An interface to the libqual polymorphic qualifier backend.

#ifndef LIBQUAL_IFACE0_H
#define LIBQUAL_IFACE0_H

// This file includes only libqual/libqual.h.  Since oink headers depend on
// this, it's better to minimize libqual dependenies.

namespace LibQual {
  extern "C" {
#include "libqual/libqual.h"
  }
};

#endif // LIBQUAL_IFACE0_H
