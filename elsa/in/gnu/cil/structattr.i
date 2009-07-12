# 1 "structattr.c"
 
 

struct A {
  int x;
} __attribute__((packed));
struct A a;

struct B {
  int x;
} __attribute__((packed)) b;


 
 
 
 

struct __attribute__((packed)) C {
  int x;
};
struct C c;

struct __attribute__((packed)) D {
  int x;
} d;


 
 
 
 

 
struct __attribute__((__packed__)) {
    int i;
    char c;
} e;


typedef unsigned long ULONG;
typedef int WCHAR;
typedef struct __attribute__((packed, aligned(4))) _INFORMATION {
     ULONG FileAttributes;
     ULONG FileNameLength;
     WCHAR FileName[1];
} INFORMATION, *PINFORMATION;

INFORMATION i;
PINFORMATION pi;

int main()
{
  return 0;
}
