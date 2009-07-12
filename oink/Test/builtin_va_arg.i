//  /home/dsw/oink_extra/ballAruns/tmpfiles/./ElectricFence-2.2.2-15/print-RlLE.i:3052:68: Parse error (state 491) at void

typedef unsigned long ef_number;
typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;
static void printNumber(ef_number number, ef_number base) {}
void EF_Printv(const char * pattern, va_list args) {
  printNumber((ef_number)
              __builtin_va_arg(args, void *)
              ,0x10);
}

