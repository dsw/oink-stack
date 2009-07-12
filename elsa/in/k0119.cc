// Umesh says this dies during oinkx serialization on a 64-bit machine,
// although I (Karl) can't reproduce it (on IA-64).

typedef unsigned long long uint64;
uint64 htonll(uint64 x) {
   return (__extension__ ({
       union {
           unsigned long int __l[2];
       } __w;
   }
  ));
}
