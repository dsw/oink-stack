//  /home/dsw/ballA/./anaconda-9.0-4/isys-YMT2.i:11805:48: Parse error (state 521) at ;

// note the multiple semi-colons

struct knownDevices {};
typedef struct {
  ;;                            // two semis here
  int ob_refcnt; struct _typeobject *ob_type;; // two semis here
  struct knownDevices list;
  ;;;                           // three here!
  void f(int x) {
  } // inline function def with no semi
  int x;
  void f2(int x) {
  };                            // inline function def with semi
  int x2;
  void f3(int x) {
  } // inline function def at end with no semi
} probedListObject;
