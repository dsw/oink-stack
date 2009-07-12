//  extern int read(int __fd , void $tainted *__buf , unsigned int __nbytes ) ;
//  extern int read(int __fd , void $untainted  *__buf , unsigned int __nbytes ) ;
extern int read(int __fd , void *__buf , unsigned int __nbytes ) ;
int read(int fildes, void $tainted * buf, int nbyte);
int main() {
  int a;
  int *b;
  int c;
  int d;
  read(a, b, c, d);
}
