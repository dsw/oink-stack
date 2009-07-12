// from a2ps-4.13b-28/buffer-T0Z6.i

// __FUNCTION__ and __PRETTY_FUNCTION__ are very similar to the
// __func__ variable, but elsa was treating them as tokens; see note
// in elsa/cc_tcheck.cc
void option_string_to_eol (const char *option, const char *arg)
{
  __func__;
  __FUNCTION__;
  __PRETTY_FUNCTION__;
}
