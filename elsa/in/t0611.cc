// t0611.cc
// define primary, then use it, then define a specialization that
// would have been used

// invalid, rejected by ICC, accepted by GCC (when the error is
// uncommented)

// primary
template <class T>
struct B
{};

// use; should provoke error
//ERROR(1): B<int*> b;

// explicit (partial) specialization
template <class T>
struct B<T*>
{};

// EOF
