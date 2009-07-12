// check a standard copying idiom

int main() {
  int $tainted *in = new int[100];
  int $untainted *out = new int[100]; 
  while (*in) {
    switch (*in) {
    case '\\': in++; *out++ = *in++; break;
    case '$': // ...
    default: *out++ = *in++;
    }
  }
  return 0;
}
