// t0282.cc
// playing with computation of built-in operator return types

// need an operand that will trigger overload resolution
enum E { e };
int i;
unsigned int ui;
long l;
unsigned long lu;

void f()
{
  __elsa_checkType(e + i,  (int)0);
  __elsa_checkType(e + ui, (unsigned)0);
  __elsa_checkType(e + l,  (long)0);
  __elsa_checkType(e + lu, (unsigned long)0);

  __elsa_checkType(e - ui, (unsigned)0);
}



// more variety!
struct Char {
  operator char ();
} c2;

struct Unsigned {
  operator unsigned ();
} u2;

void g()
{
  __elsa_checkType(c2 + i,  (int)0);
  __elsa_checkType(c2 + c2, (int)0);     // char + char = int
  
  __elsa_checkType(u2 + l, (unsigned long)0);
}



