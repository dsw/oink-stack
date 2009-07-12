// htonll.cc
// htonll definition on some machine

typedef unsigned long long uint64;

inline uint64 htonll(uint64 x) 
{ 
  return (__extension__ ({ 
    union {
      __extension__ unsigned long long int __ll; 
      unsigned long int __l[2]; 
    } __w, __r;
    if (__builtin_constant_p (x))
      __r.__ll = ((((x) & 0xff00000000000000ull) >> 56) |
                  (((x) & 0x00ff000000000000ull) >> 40) |
                  (((x) & 0x0000ff0000000000ull) >> 24) |
                  (((x) & 0x000000ff00000000ull) >> 8) |
                  (((x) & 0x00000000ff000000ull) << 8) |
                  (((x) & 0x0000000000ff0000ull) << 24) |
                  (((x) & 0x000000000000ff00ull) << 40) |
                  (((x) & 0x00000000000000ffull) << 56) );
    else { 
      __w.__ll = (x); 
      __r.__l[0] = (__extension__ ({ 
        register unsigned int __v; 
        if (__builtin_constant_p (__w.__l[1])) 
          __v = ((((__w.__l[1]) & 0xff000000) >> 24) |
                 (((__w.__l[1]) & 0x00ff0000) >> 8) |
                 (((__w.__l[1]) & 0x0000ff00) << 8) | 
                 (((__w.__l[1]) & 0x000000ff) << 24));
        else 
          __asm__ __volatile__ (
            "rorw $8, %w0;" 
            "rorl $16, %0;" 
            "rorw $8, %w0" 
          : "=r" (__v)
          : "0" ((unsigned int) (__w.__l[1])) 
          : "cc"); 
          __v; 
      })); 
      __r.__l[1] = (__extension__ ({ 
        register unsigned int __v; 
        if (__builtin_constant_p(__w.__l[0])) 
          __v = ((((__w.__l[0]) & 0xff000000) >> 24) | 
                 (((__w.__l[0]) & 0x00ff0000) >> 8) | 
                 (((__w.__l[0]) & 0x0000ff00) << 8) | 
                 (((__w.__l[0]) & 0x000000ff) << 24)); 
        else 
          __asm__ __volatile__ (
            "rorw $8, %w0;" 
            "rorl $16, %0;" 
            "rorw $8, %w0" 
          : "=r" (__v) 
          : "0" ((unsigned int) (__w.__l[0])) 
          : "cc"); 
          __v; 
      })); 
    } 
    __r.__ll; 
  })); 
}
