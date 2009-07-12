//  * = atomic (int * = int)

// This one demonstrates assignment to a larger type causing fan-out;
// it is necessary to complement what would happen if it were paired
// with an assignment going the other way.
int main() {
  int $untainted * x;
  int $tainted y;
  x = y;                        // bad
}
