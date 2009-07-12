// template constructor instantiated using function type

// originally found in package 'wvstreams_4.0.2-4'

// Assertion failed: var0inst != NULL && "f1c15444-8783-4296-981f-e3908d7cb1b4", file overload.cc line 351
// Failure probably related to code near a.ii:10:12
// current location stack:
//   a.ii:10:12
//   a.ii:9:12
//   a.ii:9:5

// ERR-MATCH: Assertion failed:.*f1c15444-8783-4296-981f-e3908d7cb1b4

struct Callback {
  template<typename Functor>  Callback(const Functor& func) {}
};

void foo(int x) {}

int main() {
  Callback cb(foo);
}
