// restrict reference

// ERR-MATCH: Parse error .state 399. at restrict

// first seen in package 'cppunit'

void foo(int & __restrict x) {}
