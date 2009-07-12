// inline explicit instantiations

// ERR-MATCH: c24a5f9c-edbd-4945-a56a-ed73a1d6a0fa

// In state 161, I expected one of these tokens:
//   <name>,
// k0107.cc:5:17: Parse error (state 161) at class

template <class C>
struct T {};

inline template class T<int>;
