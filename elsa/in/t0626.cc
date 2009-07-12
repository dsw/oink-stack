// t0626.cc
// explicit specialization where an empty template argument list is supplied

// Reported by Umesh Shankar.

template <class T>
void f(T* p);

template<>
void f<>(int* p) {
}

// EOF
