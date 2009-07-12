int main() {
  int x[3];
  x[1] = ($tainted int) 17;
  int $untainted y = x[2];      // should fail: all array elements are unified
//    options.identity_files[tmp___5] = xstrdup((char const   *)BSDoptarg);
}
