// This causes an error due to unknown locations if you turn off the
// check for unknown location that avoids pam_add_overlay_file in
// qual_walk.cc:Declarator::qual().
void f();
int main() {
  (void) f();
  (void) f();
}

