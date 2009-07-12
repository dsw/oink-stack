// The problem is that read takes an argument and the type that it is
// being cast to does not.

//  extern int read(int __fd , void *__buf , int __nbytes ) ;
extern int read(int fd) ;
//  void atomicio(int (*f)()) {
void atomicio(int (*f)()) {
}
int main() {
//      atomicio((int (*)())(& read));
    atomicio((int (*)())(& read));
}
