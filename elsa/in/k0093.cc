// tertiary operator implicitly casting one pointer operand to bool

// originally found in package 'vdr'

// a.ii:5:8: error: incompatible ?: argument types `struct S1 *' and `bool'

// ERR-MATCH: incompatible [?]: argument types

int main() {
  struct S1 * x;
  bool b = true ? x : false;
}
