//  /home/dsw/ballA/a2ps-4.13b-28/ccBp1p0u-TEXs.i:5:1: error: constructors must be class members

// In K&R C, a function with no return value implicitly returns an
// int.  This one parses as a C++ constructor so we have to catch it
// in the user action.

main()
{
  return(0);
}
