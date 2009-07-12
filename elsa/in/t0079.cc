// in/t0079.cc
// problem with templatized forward decl?
// no, was a problem with templatized prototypes

class istream;

template<class TP> class smanip;
//template<class TP> class smanip {};

template<class TP>
inline istream& operator>>(istream& i, const smanip<TP>& m);
//int foo(smanip<TP> &m);

typedef smanip<int> smanip_int;

template<class TP> class smanip {
public:
  smanip *whatever;
  smanip<TP> *whatever2;
};

void f()
{
  smanip_int s;
  s.whatever;
}

