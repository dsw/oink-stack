// assigning from an array to a pointer doesn't seem to propagate the
// qualifiers.  Example from Matt.

typedef int ssize_t;
typedef unsigned int size_t;
ssize_t read(int __fd , char $tainted *__buf , size_t __nbytes) {}
int main(){
  char buf[256];
  char* p = buf;
  int count = read(0, p, 16); // bad
  char $untainted *s = p;
  return 0;
}
