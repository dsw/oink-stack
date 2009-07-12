// t0610.cc
// example that requires allowing a dependent array size to
// bind to another expression (another dependent array size);
// reported by Umesh Shankar

// The overload prefers the second declaration because this first
// declaration can be made equal to the second by substituting "const
// TT" for T" and "NN" for "N" (but the reverse is not possible), so
// that means the second is "more specialized".
template <typename T, int N>
int ArraySizeHelper(T (&array)[N]);

template <typename TT, int NN>
int ArraySizeHelper(const TT (&array)[NN]);

static const char* ary[] = {
  "a",
  "b"
};

int f() {
  return sizeof(ArraySizeHelper(ary));
}

// EOF
