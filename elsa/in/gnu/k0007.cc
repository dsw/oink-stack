// __builtin_frame_address

// originally found in package 'aleph_0.9.0-2'

// a.ii:5:14: error: there is no function called `__builtin_frame_address'

// ERR-MATCH: there is no function called `__builtin_frame_address'

int main()
{
  void * p;
  p = __builtin_frame_address(0);
  p = __builtin_return_address(0);
}
