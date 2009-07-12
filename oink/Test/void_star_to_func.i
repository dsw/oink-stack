int ldap_ufn_timeout( void *tvparam );
void f() {
  int foo;
  // the type of the whole conditional is taken from the 'then'
  // clause, and is therefore a function type (not a pointer to
  // function type), so when the edge is inserted for the 'else'
  // clause, an edge flows from a void* to a function
  foo ? ldap_ufn_timeout : ((void *)0);
}
