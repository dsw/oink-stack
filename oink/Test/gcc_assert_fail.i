// from a2ps-4.13b-28/buffer-T0Z6.i

// elsa doesn't know __assert_fail
void option_string_to_eol (const char *option, const char *arg)
{
  char *name;
  __assert_fail ("", "buffer.c", 117, name);
  // __func__ is an independent source of complexity
//    __assert_fail ("", "buffer.c", 117, __func__);
}
