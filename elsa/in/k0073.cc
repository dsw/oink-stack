// compound initializer for instantiated template struct

// originally found in package 'speech-tools_1:1.2.3-8'

// a.ii:5:15: error: can't initialize memberless aggregate S<int /*anon*/>

// ERR-MATCH: can't initialize memberless aggregate

template<class T> struct S { int foo; };

static S<int> estfile_names[] = { { 42 } };
