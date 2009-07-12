//  extern int read(int __fd , void $tainted *__buf , unsigned int __nbytes ) ;
//  extern int read(int __fd , void $untainted  *__buf , unsigned int __nbytes ) ;

// swap these two and see the error come and go.
// demonstrates that the first declaration seen is the one used.

//  int read(int __fd , void *__buf , unsigned int __nbytes, int d ) ;
int read(int __fd , void *__buf , unsigned int __nbytes ) ;

int read(int __fd , void *__buf , unsigned int __nbytes) {
  return __fd;
}

//  int read(int fildes, void $tainted * buf, int nbyte);
int main() {
  int $tainted a;
  int *b;
  int c;
  int d;
  int e;
//    e = read(a, b, c, d);
  e = read(a, b, c);
//    read(a, b, c);
}
