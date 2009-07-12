/* ./amanda-2.4.3-4/amcheck-hUQ3.i.c_out ; was segfaulting */

int start_server_check(fd, do_localchk, do_tapechk)
    int fd;
{
  do_localchk++;
}

// this doesn't work yet since it parses as a normal function and not
// a K&R one.
/*  int main(argc) */
/*  { */
/*    int x = argc; */
/*  } */
