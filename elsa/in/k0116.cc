// template parameter default instantiating template class

// Assertion failed: !parameterizedEntity && "4d621e9b-fdc9-4646-918c-76bd950d191c" , file cc_scope.cc line 1217

// scim_1.0.2-3/scim_slot.cpp.313b4664519cb00a19e4d377049f0f0a.ii

// ERR-MATCH: 4d621e9b-fdc9-4646-918c-76bd950d191c

template <class T0>
struct S1 {
};

template <class T1 = class S1<int> >
struct S2 {
};

int main()
{
  S2<> s2;
}
