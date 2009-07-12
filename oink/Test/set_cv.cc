// TypeFactory_O::setCVQualifiers() now calls the new setCVFlag(cv),
// not addCVFlag, on newBaseType_O as sometimes we want to subtract an
// existing flag, not add one

void *memcpy (void *);

template <class T> 
void ConvertBreaks (const T *) {
  T *r;
  memcpy(r);
}

void ConvertLineBreaks() {
  const char *a;
  ConvertBreaks (a);
}
