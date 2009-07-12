// expression in compound initializer array index

// originally found in package 'tcc'

// ERR-MATCH: 48c58fcf-900a-4b17-bf13-17a2c93d799d

char s[1] = { [(0+0)] = 1 };
