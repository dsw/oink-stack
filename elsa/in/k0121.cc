// For testing nested template delimiter pretty printing.  From Evan Driscoll.

// http://www.cubewano.org/oink/ticket/123

template <class T>
class C {};

template <class T1, class T2 = C<T1> >
class basic_string;
