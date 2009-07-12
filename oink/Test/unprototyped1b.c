char * strcpy(char * d, char * s)
{
  char * d0 = d;
  while (*s) { *d++ = *s++; }
  return d0;
}
