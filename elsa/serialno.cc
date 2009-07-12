// serialno.cc
// code for serialno.h

#include "serialno.h"         // this module
#include "trace.h"            // tracingSys
#include "fstream.h"          // ifstream


// -------------------- serial numbers ON --------------------
#if USE_SERIAL_NUMBERS

SerialBase::SerialBase()
  : serialNumber(incSerialNumber())
{}


// I define this function so that even the copy ctor will get
// and use a unique serial number
SerialBase::SerialBase(SerialBase const &obj)
  : serialNumber(incSerialNumber())
{}


SerialBase& SerialBase::operator= (SerialBase const &)
{
  // do *not* copy the serial number
  return *this;
}


// FIX: given that I keep counting across multiple files now and given
// that we will be analyizing huge inputs, it might help if this were
// unsigned; then again, you couldn't catch a rollover
int globalSerialNumber = 0;

// initialize the global serial number; this is for multi-file
// symmetry breaking
class GlobalSerialNoInit {
  char const * const filename;

  public:
  GlobalSerialNoInit(char const * const filename0)
    : filename(filename0)
  {
    if (tracingSys("serialno-read")) {
      try {
        ifstream in(filename);
        in >> globalSerialNumber;
      } catch(...) {}
    }
    if (tracingSys("serialno-announce")) {
      cout << "starting with globalSerialNumber " << globalSerialNumber << endl;
    }
  }

  ~GlobalSerialNoInit() {
    if (tracingSys("serialno-write")) {
      ofstream out(filename);
      out << globalSerialNumber << endl;
    }
    if (tracingSys("serialno-announce")) {
      cout << "ending with globalSerialNumber " << globalSerialNumber << endl;
    }
  }
};
GlobalSerialNoInit gsnInit(".serialno"); // I need it to run at initialization time

int incSerialNumber()
{
  // put the value into a local variable so that (at least when
  // optimization is turned off) there is an easy name to use
  // for conditional breakpoints
  int sn = globalSerialNumber++;

  // NOTE: put the breakpoint ABOVE the previous line, NOT HERE!
  return sn;
}

void printSerialNo(stringBuilder &sb, char const *pre, int num, char const *post)
{
  if (tracingSys("serialNumbers")) {
    sb << pre << num << post;
  }
}


#endif // USE_SERIAL_NUMBERS



// -------------------- serial numbers OFF --------------------
// nothing..


// EOF
