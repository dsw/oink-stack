static void log_ultrastor_abort(int command)
{
  static char fmt[80] = "abort %d (%x); MSCP free pool: %x;";
  register int i;
  unsigned long flags;
  __asm__ __volatile__("pushfl ; popl %0":"=g" (flags): );
  __asm__ __volatile__("cli": : :"memory");

  for (i = 0; i < 16; i++)
    {
      fmt[20 + i*2] = ' ';
    }
  fmt[20 + 16 * 2] = '\n';
  fmt[21 + 16 * 2] = 0;
  __asm__ __volatile__("pushl %0 ; popfl": :"g" (flags):"memory", "cc");
}
