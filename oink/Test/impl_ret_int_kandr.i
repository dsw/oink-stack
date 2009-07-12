//  /home/dsw/ballA/./am-utils-6.0.9-2/cc0AewjJ-dase.i:4747:1: Parse error (state 267) at int

// In K&R C, a function with no return value implicitly returns an
// int.  This one parses as some previously undefined thing, so we
// have to modify the grammar.

main(argc)
  int argc;
{
  return(0);
}
