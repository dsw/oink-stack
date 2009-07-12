// ERR-MATCH: 93915d80-5dc6-4459-b279-ce978e8473c8

// oink: Assertion failed: Not implemented: data flow for BIN_MINIMUM,
// BIN_MAXIMUM (93915d80-5dc6-4459-b279-ce978e8473c8), file
// dataflow_visitor.cc line 807

int main() {
  int foo, bar;
  foo <? bar;
  foo >? bar;
}
