// sm_strstream.h      see license.txt for copyright and terms of use
// portable header for c++ strstream

#ifndef SM_STRSTREAM_H
#define SM_STRSTREAM_H

#if defined(__GNUC__) && (__GNUC__ > 2)
  // gcc-3 doesn't have istrstream (but it is standard!), so fake it
#  include <sstream>       // istringstream

#  undef string            // in case the string->mystring definition is active
#  include <string>        // std::string
  
  typedef std::istringstream istrstream;
  typedef std::ostringstream ostrstream;

  // define fake istrstream-constructor arguments
#  define wrap_istrstream_args(buf, len) std::string(buf, len)
  // define wrapper for ostringstream::str
#  define wrap_ostrstream_str(o) (o).str().c_str();
#else
  // should work on any compiler that implements the C++98 standard
  // (istrstream is deprecated but still standard)
#  include <strstream.h>   // istrstream
#  define wrap_istrstream_args(buf, len) buf, len
#  define wrap_ostrstream_str(o) (o).str();
#endif


#endif // SM_STRSTREAM_H
