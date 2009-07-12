// complex double * int

// originally found in package 'snd_7.8-1'

// b.ii:7:19: error: invalid complex arithmetic operand types `double _Complex &' and `int &'

// ERR-MATCH: invalid complex arithmetic operand types

int main()
{
  _Complex double a;
  int b;
  _Complex double c = a * b;
  _Complex double d = b * a;
}
