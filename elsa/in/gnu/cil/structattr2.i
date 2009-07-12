# 1 "structattr2.c"
 
 
 

 
const struct c  { int a; } b, e;

 
struct c d;


 
struct c2  { int a; } const b2, e2;

struct c2 d2;


 
struct c3  { int a; } const;
struct c3 b3, e3;

const struct c4  { int a; };
struct c4 b4, e4;

struct __attribute__((packed)) c5 { int a; } b5, e5;
struct c5 d5;

struct c6 { int a; } __attribute__((packed)) b6, e6;
struct c6 d6;

struct c7 { int a; } __attribute__((packed));
struct c7 b7;

int main() { return 0; }

