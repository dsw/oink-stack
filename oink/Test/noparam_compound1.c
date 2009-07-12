// ERR-MATCH: 0c09b0c3-344d-400a-be48-53961099883a

// qual: Assertion failed: !srcParamValue->t()->isCompoundType() &&
// "0c09b0c3-344d-400a-be48-53961099883a", file dataflow_ty.cc line 377

struct S {};

int fetch();

int fetch(struct S s)
{
}
