//ssize_t atomicio(ssize_t (*f)() , int fd , void *_s , size_t n ) ;
//  ssize_t atomicio(ssize_t (*f)() , int fd , void *_s , size_t n ) 
//  { char *s ;
//    ssize_t res ;
//    ssize_t pos ;
//    int *tmp ;
//    int *tmp___0 ;
//    int *tmp___1 ;

//    {
//  # 40 "atomicio.c"

//    s = (char *)_s;
//  # 41 "atomicio.c"

//    pos = 0;
//  # 43 "atomicio.c"

//    while (n > (unsigned int )pos) {
//  # 44 "atomicio.c"

//      res = ((*((ssize_t (*)(int a1 , char *a2 , size_t a3 ))f)))(fd, s + pos, n - (unsigned int )pos);
//  # 45 "atomicio.c"

//      switch (res) {
//      case -1: 
//  # 48 "atomicio.c"

//      tmp = __errno_location();
//  # 48 "atomicio.c"

//      if ((*tmp) == 4) {
//  # 48 "atomicio.c"

//        continue;
//      } else {
//  # 48 "atomicio.c"

//        tmp___0 = __errno_location();
//  # 48 "atomicio.c"

//        if ((*tmp___0) == 11) {
//  # 48 "atomicio.c"

//          continue;
//        } else {
//  # 48 "atomicio.c"

//          tmp___1 = __errno_location();
//  # 48 "atomicio.c"

//          if ((*tmp___1) == 11) {
//  # 48 "atomicio.c"

//            continue;
//          }
//        }
//      }
//      case 0: ;
//  # 54 "atomicio.c"

//      return (res);
//      default: 
//  # 56 "atomicio.c"

//      pos += res;
//      }
//    }
//  # 59 "atomicio.c"

//    return (pos);
//  }
//  }

// ****************
//  typedef int __ssize_t;
//  typedef __ssize_t ssize_t;
//  typedef unsigned int size_t;
//  extern ssize_t read(int __fd , void *__buf , size_t __nbytes ) ;
extern int read(int __fd , void *__buf , int __nbytes ) ;
//  ssize_t atomicio(ssize_t (*f)() , int fd , void *_s , size_t n) {
//  ssize_t atomicio(ssize_t (*f)()) {
//  int atomicio(int (*f)()) {
void atomicio(int (*f)()) {
//    return 1;                     // dummy
}

int main() {
//    int connection_in ;
//    char buf[256] ;
//    int tmp___1 ;
//    int i ;
//    tmp___1 = atomicio((ssize_t (*)())(& read), connection_in, (void *)(& buf[i]), 1U);
//    tmp___1 = atomicio((ssize_t (*)())(& read));
//    tmp___1 =
    atomicio((int (*)())(& read));
}

// passes ****************
//  void f(int *x) {}
//  int main() {
//    int *y = 0;
//    f(y);
//  }
