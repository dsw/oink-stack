typedef int __ssize_t;
typedef __ssize_t ssize_t;
typedef unsigned int size_t;
extern ssize_t read(int __fd , void *__buf , size_t __nbytes ) ;
// extern int read(int __fd , void *__buf , int __nbytes ) ;
ssize_t atomicio(ssize_t (*f)() , int fd , void *_s , size_t n) {
  return 1;                     // dummy
}

int main() {
  int connection_in ;
  char buf[256] ;
  int tmp___1 ;
  int i ;
  tmp___1 = atomicio((ssize_t (*)())(& read), connection_in, (void *)(& buf[i]), 1U);
}
