//  /home/dsw/oink_extra/ballAruns/tmpfiles/./arts-1.1-7/gsldatahandle-mad-04hG.i:2145:107: Parse error (state 222) at <string literal>: "fpatan"

extern __inline double atan2 (double __y, double __x)
{
  register long double __value;
  __asm __volatile__ ("fpatan":"=t" (__value):"0" (__x), "u" (__y):"st(1)");
  return __value;
}
