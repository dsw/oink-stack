void register_var(int **);
void unregister_var(int **);

int $!regme *v;                 // globals don't have to be registered

void foo(int $!regme *v2) {     // parameters neither
  float x;
  int $!regme v3;               // not enough layers of pointers
  while (1) {}
  int $!regme *v4;              // this one must be registered
  while (1) {}                  // there is some delay
  register_var(&v4);            // and finally it is
  v4;                           // and used
  unregister_var(&v4);          // and unregistered
  int $!regme **v5;             // too many layers of pointers
}
