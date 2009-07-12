// ERR-MATCH: c9e8f15c-0e47-4852-b7f1-1ede70a102dd
// see also 04c1fde6-5c97-4f2c-a176-b8e4f2bc27de

template <typename T> struct T1 {
  T1() {
  }
};
extern template struct T1<int>;

