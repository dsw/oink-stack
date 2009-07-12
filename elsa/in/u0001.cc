// u0001.cc:8:22: error: call site name lookup failed to yield any candidates;
// last candidate was removed because: incompatible call site args

template <typename T, unsigned int N>
char (&ArraySize(T (&array)[N]))[N];

int attrs_[32];

void test() {
   int size = sizeof(ArraySize(attrs_));
}
