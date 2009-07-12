// ERR-MATCH: b6160580-54bb-4f08-a032-a69eb4791f3b

float dummy() {}

template <class T1> struct S1 {
  typedef typename T1::footype footype;
};
