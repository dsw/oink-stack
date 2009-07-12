// t0134.c
// the exact example for transparent unions from the GCC manual:
// http://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Type-Attributes.html

// prelude declarations

union wait {
  int blah;
};

typedef int pid_t;

pid_t waitpid(pid_t pid, int *status, int options);

// actual example

typedef union
  {
    int *__ip;
    union wait *__up;
  } wait_status_ptr_t __attribute__ ((__transparent_union__));

pid_t wait (wait_status_ptr_t);

int w1 () { int w; return wait (&w); }
int w2 () { union wait w; return wait (&w); }

pid_t wait (wait_status_ptr_t p)
{
  return waitpid (-1, p.__ip, 0);
}

// EOF
