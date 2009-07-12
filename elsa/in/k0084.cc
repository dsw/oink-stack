// complex literals

// originally found in package 'snd_7.8-1'

// a.ii:6:7: error: two adjacent nonseparating tokens
// In state 4, I expected one of these tokens:
//   (, ), [, ], ->, ., +, -, ++, --, &, *, .*, ->*, /, %, <<, >>, <, <=, >, >=, ==, !=, ^, |, &&, ||, ?, :, =, *=, /=, %=, +=, -=, &=, ^=, |=, <<=, >>=, ,, ..., ;, }, __attribute__, <?, >?,
// a.ii:6:9: Parse error (state 4) at <name>: i

// ERR-MATCH: two adjacent nonseparating tokens

int main()
{
  _Complex double d;
  d = 42i;
  d = 42.0i;
  d = 42.0fi;
}
