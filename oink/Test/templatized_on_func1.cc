// ERR-MATCH: 750e1581-41c2-4079-b8e1-482a5c78404e

// qual: Assertion failed: !serializeOracle ||
// serializeOracle->shouldSerialize(sta->value.v) &&
// "750e1581-41c2-4079-b8e1-482a5c78404e", file ../elsa/xml_type_writer.cc
// line 666

// see also elsa/in/t0561.cc

int foo();

typedef int (*Func)();

template <Func f>
struct S {
};

int main() {
  S<&foo> s;
}
