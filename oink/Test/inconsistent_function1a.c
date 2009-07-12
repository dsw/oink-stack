// ERR-MATCH: inconsistently a function across translation units

// Test that we can link the following together:
//    1) a data item in C
//    2) a function in C++

int foo() {}

int main() {}
