// ERR-MATCH: 09706d00-1974-4571-9282-63b8eef6a833

// qual: a.i:14: Target function has fewer parameters before the ellipsis
// than source function (09706d00-1974-4571-9282-63b8eef6a833).

typedef void (*Func) ();

int connect (Func func);

void foo1 (void *a1, double a2, char $tainted a3) {}

void foo2 (void *a1, float a2, char $untainted a3) {}

void foo3 (void *a1, int a2) {}

void main() {
  connect (foo1);
  connect (foo3);
  connect (foo2);               // BAD
}
